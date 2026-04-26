<template>
    <div class="vocabulary-card" :class="{ favorite: isFavorited }">
        <div class="card-header">
            <span class="word">{{ item[1] }}</span>
            <span class="pos">[{{ item[5] }}]</span>
            <span class="romaji">{{ item[2] }}</span>
            <button class="favorite-btn" @click.stop="toggleFavorite" :class="{ active: isFavorited }">
                <span class="star-icon" :style="{ '--star-icon-url': `url(${starSolidIcon})` }"></span>
            </button>
        </div>
        <div class="card-content">
            <span class="category">{{ getCategoryName(item[6]) }}</span>
            <span class="meaning">{{ item[3] }}</span>
        </div>
    </div>
</template>

<script setup>
import { ref, computed } from 'vue';
import { SCENE_CATEGORIES } from '../../../../assets/data/japanese/vocabulary/categories.js';
import starSolidIcon from '../../../../assets/icons/star-solid.svg?url';

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
    border-radius: var(--radius-md);
    padding: var(--spacing-sm);
    cursor: pointer;
    transition: all 0.2s ease;
    border: 1px solid var(--border-color);
    flex: 1;
    min-width: 180px;
    max-width: 24%;
    display: flex;
    flex-direction: column;
    gap: var(--spacing-xs);
}

.vocabulary-card:hover {
    background: var(--bg-subtle);
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.06);
    border-color: var(--accent-primary);
}

.vocabulary-card.favorite {
    border-color: var(--warning);
    background: color-mix(in srgb, var(--warning) 6%, var(--bg-elevated));
}

.card-header {
    display: flex;
    align-items: center;
    gap: var(--spacing-xs);
    margin-bottom: var(--spacing-xs);
}

.word {
    font-size: var(--font-size-lg);
    font-weight: 700;
    color: var(--text-primary);
    line-height: 1;
}

.pos {
    font-size: var(--font-size-md);
    color: var(--text-muted);
    font-weight: 500;
    flex-shrink: 0;
    margin-left: 2px;
}

.romaji {
    font-size: var(--font-size-md);
    color: var(--accent-primary);
    font-weight: 500;
    flex-shrink: 0;
}

.favorite-btn {
    background: none;
    border: none;
    cursor: pointer;
    padding: 6px;
    margin-left: auto;
    color: var(--text-muted);
    transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);
    border-radius: var(--radius-full);
    display: flex;
    align-items: center;
    justify-content: center;
    flex-shrink: 0;
    opacity: 0;
    transform: scale(0.8);
}

.vocabulary-card:hover .favorite-btn,
.favorite-btn.active {
    opacity: 1;
    transform: scale(1);
}

.favorite-btn:hover {
    color: var(--warning);
    background: color-mix(in srgb, var(--warning) 12%, transparent);
    border-radius: 20%;
}

.favorite-btn.active {
    color: var(--warning);
    border-radius: 50%;
    animation: heartbeat 0.6s ease-in-out;
}

@keyframes heartbeat {
    0% {
        transform: scale(1);
        box-shadow: 0 0 0 0 rgba(255, 193, 7, 0.4);
    }
    30% {
        transform: scale(1.3);
        box-shadow: 0 0 0 8px rgba(255, 193, 7, 0);
    }
    100% {
        transform: scale(1);
        box-shadow: 0 0 0 0 rgba(255, 193, 7, 0);
    }
}

.star-icon {
    width: 14px;
    height: 14px;
    display: block;
    opacity: 0.3;
    background-color: var(--text-muted);
    mask-image: var(--star-icon-url);
    mask-size: contain;
    mask-repeat: no-repeat;
    mask-position: center;
}

.vocabulary-card:hover .star-icon,
.favorite-btn:hover .star-icon {
    opacity: 1;
    background-color: var(--text-muted);
}

.favorite-btn.active .star-icon {
    opacity: 1;
    background-color: var(--warning);
}

.card-content {
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: var(--spacing-xs);
    flex: 1;
    min-height: 0;
}

.meaning {
    font-size: var(--font-size-md);
    color: var(--text-secondary);
    line-height: 1.4;
    text-align: right;
    flex: 1;
    min-width: 0;
}

.category {
    font-size: 9px;
    color: var(--text-muted);
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
    flex-shrink: 0;
}

@media (prefers-color-scheme: dark) {
    .vocabulary-card {
        box-shadow: 0 1px 2px rgba(0, 0, 0, 0.15);
    }

    .vocabulary-card:hover {
        box-shadow: 0 6px 16px rgba(0, 0, 0, 0.2);
    }
}
</style>
