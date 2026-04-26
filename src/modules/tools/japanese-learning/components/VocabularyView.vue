<template>
    <div class="vocabulary-view" ref="containerRef">
        <div class="toolbar">
            <div class="toolbar-left">
                <SearchInput v-model="searchQuery" placeholder="搜索..." @input="debouncedSearch" />
                <span class="stats-inline" v-if="totalWords > 0">
                    <span class="stat-value">{{ totalWords }}</span>
                    <span class="stat-label">个单词</span>
                </span>
            </div>
            <div class="toolbar-right">
                <BaseSelect v-model="selectedLevel" @update:modelValue="handleLevelChange" :options="levelOptions" placeholder="级别" />
                <BaseSelect v-model="selectedPartOfSpeech" @update:modelValue="handlePartOfSpeechChange" :options="partOfSpeechOptions" placeholder="词性" />
                <template v-if="favoritesCount > 0">
                    <span class="stats-inline">
                        <span class="stat-value favorites">{{ favoritesCount }}</span>
                        <span class="stat-label">已收藏</span>
                    </span>
                </template>
                <BaseCheckbox v-model="showFavoritesOnly" @update:modelValue="loadVocabulary">只看收藏</BaseCheckbox>
            </div>
        </div>
        <div class="category-bar" v-if="availableCategories.length > 0">
            <button class="category-btn" :class="{ active: selectedCategories.length === 0 }" @click="clearCategories">全部</button>
            <button v-for="cat in availableCategories" :key="cat.key" class="category-btn"
                :class="{ active: selectedCategories.includes(cat.key) }" @click="toggleCategory(cat.key)">
                {{ cat.name }}
            </button>
        </div>

        <div class="vocabulary-list" v-if="words.length > 0">
            <VocabularyCard v-for="(word, idx) in words" :key="word[0]" :item="word"
                :is-favorite="isFavorite(word[0])" @toggle-favorite="handleToggleFavorite" />

            <div v-if="hasMore" class="load-more-trigger" ref="loadMoreRef">
                <div class="loading-spinner" v-if="loading">
                    <div class="spinner"></div>
                    <span>加载更多...</span>
                </div>
            </div>

            <div class="end-message" v-if="!hasMore && words.length > 0">
                已显示全部 {{ words.length }} 个单词
            </div>
        </div>

        <div class="empty-state" v-else-if="!loading">
            <div class="empty-icon">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <circle cx="11" cy="11" r="8" />
                    <path d="M21 21l-4.35-4.35" />
                </svg>
            </div>
            <div class="empty-text">未找到匹配的单词</div>
            <BaseButton @click="resetFilters">重置筛选</BaseButton>
        </div>

        <div class="loading-state" v-if="loading && words.length === 0">
            <div class="spinner large"></div>
            <span>加载中...</span>
        </div>
    </div>
</template>

<script setup>
import { ref, computed, watch, onMounted, nextTick } from 'vue';
import { useVocabularyService } from '../services/vocabularyService.js';
import { VOCABULARY_CATEGORIES, SCENE_CATEGORIES, PARTS_OF_SPEECH } from '../../../../assets/data/japanese/vocabulary/categories.js';
import SearchInput from '../../../../components/base/SearchInput.vue';
import BaseButton from '../../../../components/base/BaseButton.vue';
import BaseSelect from '../../../../components/base/BaseSelect.vue';
import BaseCheckbox from '../../../../components/base/BaseCheckbox.vue';
import VocabularyCard from './VocabularyCard.vue';

const {
    getFavorites,
    toggleFavorite,
    isFavorite: checkIsFavorite,
    getPaginatedVocabulary
} = useVocabularyService();

const levels = ['n5', 'n4', 'n3', 'n2', 'n1'];
const containerRef = ref(null);
const loadMoreRef = ref(null);

const selectedLevel = ref('n5');
const selectedCategories = ref([]);
const selectedPartOfSpeech = ref('');
const showFavoritesOnly = ref(false);
const searchQuery = ref('');
const words = ref([]);
const page = ref(0);
const totalWords = ref(0);
const hasMore = ref(true);
const loading = ref(false);
const favoritesCount = ref(0);

let searchTimeout = null;

const levelOptions = computed(() => {
    return levels.map(level => ({
        label: level.toUpperCase(),
        value: level
    }));
});

const availableCategories = computed(() => {
    const levelConfig = VOCABULARY_CATEGORIES[selectedLevel.value];
    if (!levelConfig) return [];

    return levelConfig.categories
        .filter(cat => SCENE_CATEGORIES[cat])
        .map(cat => ({
            key: cat,
            name: SCENE_CATEGORIES[cat].name
        }));
});

const partsOfSpeech = computed(() => {
    return Object.entries(PARTS_OF_SPEECH).map(([key, value]) => ({
        key,
        name: value.name
    }));
});

const partOfSpeechOptions = computed(() => {
    return [
        { label: '全部', value: '' },
        ...Object.entries(PARTS_OF_SPEECH).map(([key, value]) => ({
            label: value.name,
            value: key
        }))
    ];
});

const isFavorite = (wordId) => {
    return checkIsFavorite(wordId);
};

const handleToggleFavorite = (wordId) => {
    toggleFavorite(wordId);
    favoritesCount.value = getFavorites().length;
};

const handleLevelChange = (level) => {
    selectedLevel.value = level;
    selectedCategories.value = [];
    selectedPartOfSpeech.value = '';
    loadVocabulary();
};

const handlePartOfSpeechChange = (pos) => {
    selectedPartOfSpeech.value = pos;
    loadVocabulary();
};

const toggleCategory = (category) => {
    const idx = selectedCategories.value.indexOf(category);
    if (idx === -1) {
        selectedCategories.value.push(category);
    } else {
        selectedCategories.value.splice(idx, 1);
    }
    loadVocabulary();
};

const clearCategories = () => {
    selectedCategories.value = [];
    loadVocabulary();
};

const debouncedSearch = () => {
    if (searchTimeout) clearTimeout(searchTimeout);
    searchTimeout = setTimeout(() => {
        loadVocabulary();
    }, 300);
};

const loadVocabulary = async () => {
    loading.value = true;

    const filters = {
        categories: selectedCategories.value,
        partOfSpeech: selectedPartOfSpeech.value,
        search: searchQuery.value,
        favoritesOnly: showFavoritesOnly.value
    };

    const result = await getPaginatedVocabulary(selectedLevel.value, filters, 0);

    words.value = result.items;
    totalWords.value = result.total;
    hasMore.value = result.hasMore;
    page.value = 0;
    loading.value = false;
};

const loadMore = async () => {
    if (loading.value || !hasMore.value) return;

    loading.value = true;

    const filters = {
        categories: selectedCategories.value,
        partOfSpeech: selectedPartOfSpeech.value,
        search: searchQuery.value,
        favoritesOnly: showFavoritesOnly.value
    };

    const nextPage = page.value + 1;
    const result = await getPaginatedVocabulary(selectedLevel.value, filters, nextPage);

    words.value = [...words.value, ...result.items];
    hasMore.value = result.hasMore;
    page.value = nextPage;
    loading.value = false;
};

const resetFilters = () => {
    selectedLevel.value = 'n5';
    selectedCategories.value = [];
    selectedPartOfSpeech.value = '';
    showFavoritesOnly.value = false;
    searchQuery.value = '';
    loadVocabulary();
};

const setupInfiniteScroll = () => {
    if (!loadMoreRef.value) return;

    const observer = new IntersectionObserver(
        (entries) => {
            if (entries[0].isIntersecting && hasMore.value && !loading.value) {
                loadMore();
            }
        },
        { threshold: 0.1 }
    );

    observer.observe(loadMoreRef.value);

    return observer;
};

onMounted(() => {
    loadVocabulary();
    favoritesCount.value = getFavorites().length;
    nextTick(() => {
        setupInfiniteScroll();
    });
});
</script>

<style scoped>
.vocabulary-view {
    padding: var(--spacing-xs) var(--spacing-lg);
    max-height: 100%;
    overflow-y: auto;
}

.toolbar {
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: var(--spacing-md);
    margin-bottom: var(--spacing-md);
}

.toolbar-left {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    flex: 1;
    max-width: 400px;
}

.toolbar-left :deep(.search-input) {
    width: auto;
    flex: 1;
}

.stats-inline {
    display: inline-flex;
    align-items: center;
    gap: 4px;
    font-size: var(--font-size-sm);
    color: var(--text-tertiary);
    white-space: nowrap;
}

.stats-inline .stat-value {
    font-weight: 600;
    color: var(--accent-primary);
}

.stats-inline .stat-value.favorites {
    color: var(--warning);
}

.stats-inline .stat-label {
    color: var(--text-tertiary);
}

.toolbar-right {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
}

.toolbar-right :deep(.ui-select-wrap) {
    width: auto;
    min-width: 90px;
}

.toolbar-right :deep(.ui-checkbox-wrap) {
    white-space: nowrap;
}

.category-bar {
    display: flex;
    align-items: center;
    gap: var(--spacing-xs);
    flex-wrap: wrap;
    margin-bottom: var(--spacing-2xl);
}

.category-btn {
    padding: var(--spacing-xs) var(--spacing-sm);
    border: none;
    background: var(--bg-soft);
    color: var(--text-tertiary);
    font-size: var(--font-size-xs);
    cursor: pointer;
    border-radius: var(--radius-sm);
    transition: all var(--transition-fast);
}

.category-btn:hover {
    background: var(--bg-subtle);
    color: var(--text-secondary);
}

.category-btn.active {
    background: var(--accent-secondary);
    color: white;
}

.vocabulary-list {
    display: flex;
    flex-wrap: wrap;
    gap: var(--spacing-md);
}

.load-more-trigger {
    flex: 1;
    min-width: 100%;
    display: flex;
    justify-content: center;
    padding: var(--spacing-xl);
}

.loading-spinner {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    color: var(--text-tertiary);
}

.spinner {
    width: 20px;
    height: 20px;
    border: 2px solid var(--border-color);
    border-top-color: var(--accent-primary);
    border-radius: 50%;
    animation: spin 0.8s linear infinite;
}

.spinner.large {
    width: 40px;
    height: 40px;
    border-width: 3px;
}

@keyframes spin {
    to {
        transform: rotate(360deg);
    }
}

.end-message {
    flex: 1;
    min-width: 100%;
    text-align: center;
    padding: var(--spacing-xl);
    color: var(--text-tertiary);
    font-size: var(--font-size-sm);
}

.empty-state,
.loading-state {
    flex: 1;
    min-width: 100%;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    padding: var(--spacing-2xl);
    gap: var(--spacing-md);
}

.empty-icon {
    width: 64px;
    height: 64px;
    color: var(--text-muted);
}

.empty-icon svg {
    width: 100%;
    height: 100%;
}

.empty-text {
    color: var(--text-tertiary);
    font-size: var(--font-size-md);
}

</style>
