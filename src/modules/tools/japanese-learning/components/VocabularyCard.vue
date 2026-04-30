<template>
    <div class="vocabulary-card" :class="{ favorite: isFavorited }">
        <div class="card-header">
            <div class="word-group">
                <span class="word">{{ displayWord }}</span>
                <span class="kana" v-if="hasKanji">({{ item[1] }})</span>
            </div>
            <button class="favorite-btn" @click.stop="toggleFavorite" :class="{ active: isFavorited }">
                <span class="star-icon" :style="{ '--star-icon-url': `url(${starSolidIcon})` }"></span>
            </button>
        </div>
        <div class="card-content">
            <span class="pos">[{{ item[5] }}]</span>
            <span class="romaji">{{ item[2] }}</span>
            <span class="meaning">{{ item[3] }}</span>
        </div>
    </div>
</template>

<script setup>
import { computed } from 'vue';
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

const hasKanji = computed(() => {
    return props.item[4] && props.item[4] !== '' && props.item[4] !== props.item[1];
});

const displayWord = computed(() => {
    if (props.item[4] && props.item[4] !== '') {
        return props.item[4];
    }
    return props.item[1];
});

const toggleFavorite = () => {
    emit('toggle-favorite', props.item[0]);
};
</script>

<style scoped>
.vocabulary-card {
    background: var(--bg-elevated);
    border-radius: var(--radius-md);
    padding: var(--spacing-md);
    cursor: pointer;
    transition: all 0.2s ease;
    border: 1px solid var(--border-color);
    flex: 1;
    min-width: 200px;
    max-width: 320px;
    display: flex;
    flex-direction: column;
    gap: 4px;
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
    padding-bottom: var(--spacing-xs);
}

.word-group {
    display: flex;
    align-items: baseline;
    gap: 6px;
    min-width: 0;
    flex: 1;
    flex-wrap: nowrap;
}

.word {
    font-size: var(--font-size-lg);
    font-weight: 700;
    color: var(--text-primary);
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
}

.kana {
    font-size: var(--font-size-lg);
    color: var(--text-muted);
    white-space: nowrap;
    flex-shrink: 0;
}

.card-content {
    display: flex;
    align-items: center;
    gap: var(--spacing-xs);
    flex-wrap: wrap;
}

.pos {
    font-size: var(--font-size-md);
    color: var(--text-muted);
    font-weight: 500;
    flex-shrink: 0;
}

.romaji {
    font-size: var(--font-size-md);
    color: var(--accent-color);
    font-weight: 500;
    flex-shrink: 0;
}

.meaning {
    font-size: var(--font-size-md);
    color: var(--text-secondary);
    line-height: 1.4;
    flex: 1;
    min-width: 0;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
    text-align: right;
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

@media (max-width: 1200px) {
    .vocabulary-card {
        flex: 1 1 calc(33.33% - 12px);
    }
}

@media (max-width: 900px) {
    .vocabulary-card {
        flex: 1 1 calc(50% - 12px);
    }
}

@media (max-width: 600px) {
    .vocabulary-card {
        flex: 1 1 100%;
        max-width: 100%;
    }
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
