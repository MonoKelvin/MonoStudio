<template>
    <div class="gojuon-view">
        <div class="view-header">
            <h2 class="view-title">五十音图</h2>
            <div class="header-actions">
                <div class="kana-toggle">
                    <button class="toggle-btn" :class="{ active: activeKana === 'hiragana' }" @click="activeKana = 'hiragana'">
                        平假名
                    </button>
                    <button class="toggle-btn" :class="{ active: activeKana === 'katakana' }" @click="activeKana = 'katakana'">
                        片假名
                    </button>
                </div>
            </div>
        </div>

        <div class="gojuon-container">
            <div class="section">
                <h3 class="section-title">清音</h3>
                <div class="gojuon-table">
                    <table>
                        <thead>
                            <tr>
                                <th></th>
                                <th v-for="(vowel, idx) in vowels" :key="vowel">
                                    <div class="vowel-cell">
                                        <span class="romaji">{{ vowelDisplays[idx] }}</span>
                                    </div>
                                </th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr v-for="(row, idx) in consonants" :key="idx">
                                <th>
                                    <div class="consonant-cell">
                                        <span class="consonant">{{ row.consonant }} 行</span>
                                    </div>
                                </th>
                                <td v-for="(vowel, vowelIdx) in vowels" :key="vowelIdx">
                                    <GojuonCard v-if="row.characters[vowelIdx]" :char="row.characters[vowelIdx]" :active-kana="activeKana" @dblclick="handleDblClick" />
                                    <div v-else class="empty-cell"></div>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>

            <div class="section">
                <h3 class="section-title">浊音・半浊音</h3>
                <div class="gojuon-table">
                    <table>
                        <thead>
                            <tr>
                                <th></th>
                                <th v-for="(vowel, idx) in vowels" :key="vowel">
                                    <div class="vowel-cell">
                                        <span class="romaji">{{ vowelDisplays[idx] }}</span>
                                    </div>
                                </th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr v-for="(row, idx) in voicedConsonants" :key="idx">
                                <th>
                                    <div class="consonant-cell">
                                        <span class="consonant">{{ row.consonant }} 行</span>
                                    </div>
                                </th>
                                <td v-for="(vowel, vowelIdx) in vowels" :key="vowelIdx">
                                    <GojuonCard v-if="row.characters[vowelIdx]" :char="row.characters[vowelIdx]" :active-kana="activeKana" @dblclick="handleDblClick" />
                                    <div v-else class="empty-cell"></div>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>

            <div class="section">
                <h3 class="section-title">拗音</h3>
                <div class="gojuon-table">
                    <table>
                        <thead>
                            <tr>
                                <th></th>
                                <th v-for="(vowel, idx) in vowels" :key="vowel">
                                    <div class="vowel-cell">
                                        <span class="romaji">{{ vowelDisplays[idx] }}</span>
                                    </div>
                                </th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr v-for="(row, idx) in yoonConsonants" :key="idx">
                                <th>
                                    <div class="consonant-cell">
                                        <span class="consonant">{{ row.consonant }} 行</span>
                                    </div>
                                </th>
                                <td v-for="(vowel, vowelIdx) in vowels" :key="vowelIdx">
                                    <GojuonCard v-if="row.characters[vowelIdx]" :char="row.characters[vowelIdx]" :active-kana="activeKana" @dblclick="handleDblClick" />
                                    <div v-else class="empty-cell"></div>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>

            <div class="section">
                <h3 class="section-title">特殊音</h3>
                <div class="special-chars">
                    <GojuonCard v-for="(char, idx) in specialChars" :key="idx" :char="char" :active-kana="activeKana" @dblclick="handleDblClick" />
                </div>
            </div>
        </div>

        <GojuonDetail
            :visible="detailVisible"
            :char="selectedChar"
            :examples="selectedChar ? findExamples(selectedChar) : []"
            @update:visible="detailVisible = $event"
            @close="detailVisible = false"
        />
    </div>
</template>

<script setup>
import { ref, computed } from 'vue';
import { GOJUON_DATA, KATAKANA_EXTRA, YOON_CONSONANTS } from '../../../../assets/data/japanese/gojuon.js';
import GojuonCard from './GojuonCard.vue';
import GojuonDetail from './GojuonDetail.vue';

const activeKana = ref('hiragana');
const selectedChar = ref(null);
const detailVisible = ref(false);

const vowels = ['a', 'i', 'u', 'e', 'o'];

const vowelMap = {
    a: 'あ 段(a)',
    i: 'い 段(i)',
    u: 'う 段(u)',
    e: 'え 段(e)',
    o: 'お 段(o)'
};

const vowelDisplays = computed(() => {
    return vowels.map(v => vowelMap[v]);
});

const consonants = computed(() => {
    return GOJUON_DATA.rows
        .filter(row => row.name.startsWith('清音'))
        .map(row => {
            // 从行名称中提取辅音
            const consonantMatch = row.name.match(/（(.)行）/);
            return {
                ...row,
                consonant: consonantMatch ? consonantMatch[1] : ''
            };
        });
});

const voicedConsonants = computed(() => {
    return GOJUON_DATA.rows
        .filter(row => row.name.startsWith('浊音') || row.name.startsWith('半浊音'))
        .map(row => {
            // 从行名称中提取辅音
            const consonantMatch = row.name.match(/（(.)行）/);
            return {
                ...row,
                consonant: consonantMatch ? consonantMatch[1] : ''
            };
        });
});

const yoonConsonants = computed(() => YOON_CONSONANTS);

const specialChars = computed(() => {
    return KATAKANA_EXTRA.map(k => ({
        hiragana: k.hiragana || '',
        katakana: k.katakana,
        romaji: k.romaji,
        kanji: k.kanji || '',
        history: k.history || ''
    }));
});

const getVowelChar = (vowel, type) => {
    const vowelMapChar = {
        a: { hiragana: 'あ', katakana: 'ア' },
        i: { hiragana: 'い', katakana: 'イ' },
        u: { hiragana: 'う', katakana: 'ウ' },
        e: { hiragana: 'え', katakana: 'エ' },
        o: { hiragana: 'お', katakana: 'オ' }
    };
    return vowelMapChar[vowel][type];
};

const findExamples = (char) => {
    for (const row of GOJUON_DATA.rows) {
        const found = row.characters.find(c => c && c.romaji === char.romaji);
        if (found && row.examples) {
            return row.examples;
        }
    }
    return [];
};

const handleDblClick = (char) => {
    selectedChar.value = char;
    detailVisible.value = true;
};
</script>

<style scoped>
.gojuon-view {
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

.kana-toggle {
    display: flex;
    gap: var(--spacing-xs);
    background: var(--bg-soft);
    border-radius: var(--radius-md);
    padding: 2px;
}

.toggle-btn {
    padding: var(--spacing-xs) var(--spacing-md);
    border: none;
    background: transparent;
    color: var(--text-secondary);
    font-size: var(--font-size-sm);
    cursor: pointer;
    border-radius: var(--radius-sm);
    transition: all var(--transition-fast);
}

.toggle-btn:hover {
    color: var(--text-primary);
}

.toggle-btn.active {
    background: var(--accent-primary);
    color: white;
}

.gojuon-container {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-2xl);
}

.section {
    background: var(--bg-elevated);
    border-radius: var(--radius-lg);
    padding: var(--spacing-xl);
    box-shadow: var(--shadow-card);
}

.section-title {
    font-size: var(--font-size-lg);
    font-weight: 600;
    color: var(--text-primary);
    margin-bottom: var(--spacing-lg);
}

.gojuon-table {
    overflow-x: auto;
}

table {
    width: 100%;
    border-collapse: collapse;
    border-spacing: 0;
}

th, td {
    text-align: center;
    vertical-align: middle;
    padding: var(--spacing-sm);
}

thead {
    user-select: none;
    -webkit-user-select: none;
    background: var(--bg-soft);
}

tbody th {
    user-select: none;
    -webkit-user-select: none;
    background: var(--bg-soft);
}

thead th:last-child {
    border-radius: 0 var(--radius-md) var(--radius-md) 0;
}

tbody tr:last-child th {
    border-radius: 0 0 var(--radius-md) var(--radius-md);
}

thead th:first-child {
    border-radius: var(--radius-md) 0 0 0;
}

.vowel-cell {
    display: flex;
    align-items: center;
    justify-content: center;
    padding: 4px;
}

tbody tr {
    background: var(--bg-elevated);
    border-radius: var(--radius-md);
}

.romaji, .consonant {
    color: var(--text-muted);
    font-size: var(--font-size-md);
}

.consonant-cell {
    display: flex;
    align-items: center;
    justify-content: center;
    padding: var(--spacing-xs);
    min-width: 40px;
}

.kana {
    font-size: var(--font-size-lg);
    transition: all var(--transition-fast);
}

.kana.text-muted {
    color: var(--text-muted);
}

.empty-cell {
    min-height: 70px;
    border-radius: var(--radius-lg);
    border: 2px dashed var(--border-color);
    display: flex;
    align-items: center;
    justify-content: center;
    color: var(--text-muted);
}

/* 滚动条样式 */
.gojuon-view::-webkit-scrollbar {
    width: 6px;
}

.gojuon-view::-webkit-scrollbar-track {
    background: var(--bg-soft);
    border-radius: var(--radius-md);
}

.gojuon-view::-webkit-scrollbar-thumb {
    background: var(--bg-muted);
    border-radius: var(--radius-md);
}

.special-chars {
    display: flex;
    flex-direction: row;
    flex-wrap: wrap;
    gap: var(--spacing-md);
    justify-content: flex-start;
    align-items: center;
}

.special-chars .kana-card {
    flex: 0 0 auto;
}
</style>
