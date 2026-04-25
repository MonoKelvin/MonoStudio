<template>
    <div class="vocabulary-card" :class="{ favorite: isFavorited }">
        <div class="card-header">
            <div class="word-info">
                <span class="word">{{ item[1] }}</span>
                <span class="romaji">{{ item[2] }}</span>
                <span class="furigana" v-if="item[4]">{{ item[4] }}</span>
            </div>
            <button class="favorite-btn" @click.stop="toggleFavorite" :class="{ active: isFavorited }">
                <svg viewBox="0 0 24 24" fill="currentColor" class="star-icon">
                    <path
                        d="M12 2l3.09 6.26L22 9.27l-5 4.87 1.18 6.88L12 17.77l-6.18 3.25L7 14.14 2 9.27l6.91-1.01L12 2z" />
                </svg>
            </button>
        </div>
        <div class="card-body">
            <div class="meaning">{{ item[3] }}</div>
            <div class="meta">
                <span class="part-of-speech-badge">{{ item[5] }}</span>
                <span class="category">{{ getCategoryName(item[6]) }}</span>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, computed } from 'vue';
import { SCENE_CATEGORIES } from '../../../../assets/data/japanese/vocabulary/categories.js';

const props = defineProps({
    item: {
        type: Array,
        required: true
    },
    isFavorite: {
        type: Boolean,
        default: false
    }
});

const emit = defineEmits(['toggle-favorite']);

const isFavorited = computed(() => props.isFavorite);

const toggleFavorite = () => {
    emit('toggle-favorite', props.item[0]);
};

const getCategoryName = (category) => {
    return SCENE_CATEGORIES[category]?.name || category;
};
</script>

<style scoped>
.vocabulary-card {
    background: var(--bg-elevated);
    border-radius: var(--radius-lg);
    padding: var(--spacing-lg);
    cursor: pointer;
    transition: all var(--transition-normal);
    box-shadow: var(--shadow-card);
    border: 1px solid transparent;
}

.vocabulary-card:hover {
    transform: translateY(-2px);
    box-shadow: var(--shadow-elevated);
}

.vocabulary-card.favorite {
    border-color: var(--warning);
    background: linear-gradient(135deg, var(--bg-elevated) 0%, rgba(245, 158, 11, 0.05) 100%);
}

.card-header {
    display: flex;
    justify-content: space-between;
    align-items: flex-start;
    margin-bottom: var(--spacing-md);
}

.word-info {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-xs);
}

.word {
    font-size: var(--font-size-xl);
    font-weight: 600;
    color: var(--text-primary);
}

.romaji {
    font-size: var(--font-size-md);
    color: var(--accent-primary);
}

.furigana {
    font-size: var(--font-size-sm);
    color: var(--text-tertiary);
    margin-top: var(--spacing-xs);
}

.favorite-btn {
    background: none;
    border: none;
    cursor: pointer;
    padding: var(--spacing-xs);
    color: var(--text-muted);
    transition: all var(--transition-fast);
    border-radius: var(--radius-sm);
}

.favorite-btn:hover {
    color: var(--warning);
    background: var(--bg-soft);
}

.favorite-btn.active {
    color: var(--warning);
}

.star-icon {
    width: 20px;
    height: 20px;
}

.card-body {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-sm);
}

.meaning {
    font-size: var(--font-size-md);
    color: var(--text-secondary);
    line-height: 1.5;
}

.meta {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    margin-top: var(--spacing-xs);
}

.part-of-speech-badge {
    font-size: var(--font-size-xs);
    font-weight: 600;
    padding: 2px 8px;
    border-radius: var(--radius-sm);
    background: var(--accent-primary);
    color: white;
}

.part-of-speech-badge.名 {
    background: #22c55e;
}

.part-of-speech-badge.动 {
    background: #3b82f6;
}

.part-of-speech-badge.形 {
    background: #f59e0b;
}

.part-of-speech-badge.副 {
    background: #ef4444;
}

.part-of-speech-badge.助 {
    background: #8b5cf6;
}

.part-of-speech-badge.感 {
    background: #ec4899;
}

.category {
    font-size: var(--font-size-xs);
    color: var(--text-tertiary);
}
</style>
