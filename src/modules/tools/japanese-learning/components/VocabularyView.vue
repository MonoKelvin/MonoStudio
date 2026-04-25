<template>
    <div class="vocabulary-view" ref="containerRef">
        <div class="view-header">
            <h2 class="view-title">常用单词学习</h2>
            <div class="header-stats">
                <span class="stat">
                    <span class="stat-value">{{ totalWords }}</span>
                    <span class="stat-label">单词总数</span>
                </span>
                <span class="stat" v-if="favoritesCount > 0">
                    <span class="stat-value favorites">{{ favoritesCount }}</span>
                    <span class="stat-label">已收藏</span>
                </span>
            </div>
        </div>

        <div class="filters">
            <div class="filter-row">
                <div class="level-filters">
                    <button v-for="level in levels" :key="level" class="level-btn" :class="{ active: selectedLevel === level }"
                        @click="selectLevel(level)">
                        {{ level.toUpperCase() }}
                    </button>
                </div>
                <label class="favorites-toggle">
                    <input type="checkbox" v-model="showFavoritesOnly" @change="loadVocabulary" />
                    <span>只看收藏</span>
                </label>
            </div>

            <div class="category-filters" v-if="availableCategories.length > 0">
                <button v-for="cat in availableCategories" :key="cat.key" class="category-btn"
                    :class="{ active: selectedCategory === cat.key }" @click="selectCategory(cat.key)">
                    {{ cat.name }}
                </button>
            </div>

            <div class="part-of-speech-filters">
                <button v-for="pos in partsOfSpeech" :key="pos.key" class="pos-btn"
                    :class="{ active: selectedPartOfSpeech === pos.key }" @click="selectPartOfSpeech(pos.key)">
                    {{ pos.name }}
                </button>
            </div>

            <div class="search-box">
                <SearchInput v-model="searchQuery" placeholder="搜索单词、意思或罗马音..." @input="debouncedSearch" />
            </div>
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
const selectedCategory = ref(null);
const selectedPartOfSpeech = ref(null);
const showFavoritesOnly = ref(false);
const searchQuery = ref('');
const words = ref([]);
const page = ref(0);
const totalWords = ref(0);
const hasMore = ref(true);
const loading = ref(false);
const favoritesCount = ref(0);

let searchTimeout = null;

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

const isFavorite = (wordId) => {
    return checkIsFavorite(wordId);
};

const handleToggleFavorite = (wordId) => {
    toggleFavorite(wordId);
    favoritesCount.value = getFavorites().length;
};

const selectLevel = (level) => {
    selectedLevel.value = level;
    selectedCategory.value = null;
    selectedPartOfSpeech.value = null;
    loadVocabulary();
};

const selectCategory = (category) => {
    selectedCategory.value = selectedCategory.value === category ? null : category;
    loadVocabulary();
};

const selectPartOfSpeech = (pos) => {
    selectedPartOfSpeech.value = selectedPartOfSpeech.value === pos ? null : pos;
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
        category: selectedCategory.value,
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
        category: selectedCategory.value,
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
    selectedCategory.value = null;
    selectedPartOfSpeech.value = null;
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
    padding: var(--spacing-lg);
    max-height: calc(100vh - 200px);
    overflow-y: auto;
}

.view-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: var(--spacing-xl);
}

.view-title {
    font-size: var(--font-size-2xl);
    font-weight: 600;
    color: var(--text-primary);
}

.header-stats {
    display: flex;
    gap: var(--spacing-lg);
}

.stat {
    display: flex;
    flex-direction: column;
    align-items: center;
}

.stat-value {
    font-size: var(--font-size-xl);
    font-weight: 600;
    color: var(--accent-primary);
}

.stat-value.favorites {
    color: var(--warning);
}

.stat-label {
    font-size: var(--font-size-xs);
    color: var(--text-tertiary);
}

.filters {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-md);
    margin-bottom: var(--spacing-xl);
    padding-bottom: var(--spacing-lg);
    border-bottom: 1px solid var(--border-color);
}

.filter-row {
    display: flex;
    justify-content: space-between;
    align-items: center;
}

.level-filters {
    display: flex;
    gap: var(--spacing-xs);
}

.level-btn {
    padding: var(--spacing-xs) var(--spacing-md);
    border: 1px solid var(--border-color);
    background: transparent;
    color: var(--text-secondary);
    font-size: var(--font-size-sm);
    font-weight: 500;
    cursor: pointer;
    border-radius: var(--radius-md);
    transition: all var(--transition-fast);
}

.level-btn:hover {
    border-color: var(--accent-primary);
    color: var(--accent-primary);
}

.level-btn.active {
    background: var(--accent-primary);
    border-color: var(--accent-primary);
    color: white;
}

.favorites-toggle {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    cursor: pointer;
    font-size: var(--font-size-sm);
    color: var(--text-secondary);
}

.favorites-toggle input {
    width: 16px;
    height: 16px;
    cursor: pointer;
}

.category-filters {
    display: flex;
    flex-wrap: wrap;
    gap: var(--spacing-xs);
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

.part-of-speech-filters {
    display: flex;
    flex-wrap: wrap;
    gap: var(--spacing-xs);
}

.pos-btn {
    padding: var(--spacing-xs) var(--spacing-sm);
    border: 1px solid var(--border-color);
    background: transparent;
    color: var(--text-tertiary);
    font-size: var(--font-size-xs);
    cursor: pointer;
    border-radius: var(--radius-sm);
    transition: all var(--transition-fast);
}

.pos-btn:hover {
    border-color: var(--accent-primary);
    color: var(--accent-primary);
}

.pos-btn.active {
    background: var(--accent-primary);
    border-color: var(--accent-primary);
    color: white;
}

.search-box {
    max-width: 400px;
}

.vocabulary-list {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
    gap: var(--spacing-md);
}

.load-more-trigger {
    grid-column: 1 / -1;
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
    grid-column: 1 / -1;
    text-align: center;
    padding: var(--spacing-xl);
    color: var(--text-tertiary);
    font-size: var(--font-size-sm);
}

.empty-state,
.loading-state {
    grid-column: 1 / -1;
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
