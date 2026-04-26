import { N5_VOCABULARY } from '../../../../assets/data/japanese/vocabulary/n5.js';
import { N4_VOCABULARY } from '../../../../assets/data/japanese/vocabulary/n4.js';
import { N3_VOCABULARY } from '../../../../assets/data/japanese/vocabulary/n3.js';
import { N2_VOCABULARY } from '../../../../assets/data/japanese/vocabulary/n2.js';
import { N1_VOCABULARY } from '../../../../assets/data/japanese/vocabulary/n1.js';

const FAVORITES_KEY = 'japanese-vocab-favorites';
const BATCH_SIZE = 100;

let vocabularyCache = {
  n5: null,
  n4: null,
  n3: null,
  n2: null,
  n1: null
};

let loadingPromises = {
  n5: null,
  n4: null,
  n3: null,
  n2: null,
  n1: null
};

export function useVocabularyService() {
  const getFavorites = () => {
    try {
      const stored = localStorage.getItem(FAVORITES_KEY);
      return stored ? JSON.parse(stored) : [];
    } catch {
      return [];
    }
  };

  const saveFavorites = (favorites) => {
    localStorage.setItem(FAVORITES_KEY, JSON.stringify(favorites));
  };

  const toggleFavorite = (wordId) => {
    const favorites = getFavorites();
    const index = favorites.indexOf(wordId);
    if (index > -1) {
      favorites.splice(index, 1);
    } else {
      favorites.push(wordId);
    }
    saveFavorites(favorites);
    return favorites;
  };

  const isFavorite = (wordId) => {
    const favorites = getFavorites();
    return favorites.includes(wordId);
  };

  const loadVocabulary = async (level) => {
    if (vocabularyCache[level]) {
      return vocabularyCache[level];
    }

    if (loadingPromises[level]) {
      return loadingPromises[level];
    }

    loadingPromises[level] = (async () => {
      try {
        switch (level) {
          case 'n5':
            vocabularyCache[level] = N5_VOCABULARY;
            break;
          case 'n4':
            vocabularyCache[level] = N4_VOCABULARY;
            break;
          case 'n3':
            vocabularyCache[level] = N3_VOCABULARY;
            break;
          case 'n2':
            vocabularyCache[level] = N2_VOCABULARY;
            break;
          case 'n1':
            vocabularyCache[level] = N1_VOCABULARY;
            break;
          default:
            vocabularyCache[level] = [];
        }
        return vocabularyCache[level];
      } catch (error) {
        console.error(`Failed to load vocabulary for ${level}:`, error);
        return [];
      }
    })();

    const result = await loadingPromises[level];
    loadingPromises[level] = null;
    return result;
  };

  const filterVocabulary = (vocabulary, filters) => {
    return vocabulary.filter(item => {
      if (filters.partOfSpeech && item[5] !== filters.partOfSpeech) {
        return false;
      }
      if (filters.categories && filters.categories.length > 0) {
        if (!filters.categories.includes(item[6])) {
          return false;
        }
      }
      if (filters.search) {
        const query = filters.search.toLowerCase();
        const matchWord = item[1].toLowerCase().includes(query);
        const matchMeaning = item[3].toLowerCase().includes(query);
        const matchRomaji = item[2].toLowerCase().includes(query);
        if (!matchWord && !matchMeaning && !matchRomaji) {
          return false;
        }
      }
      if (filters.favoritesOnly) {
        if (!isFavorite(item[0])) {
          return false;
        }
      }
      return true;
    });
  };

  const getPaginatedVocabulary = async (level, filters, page = 0) => {
    const vocabulary = await loadVocabulary(level);
    const filtered = filterVocabulary(vocabulary, filters);
    const start = page * BATCH_SIZE;
    const end = start + BATCH_SIZE;
    return {
      items: filtered.slice(start, end),
      total: filtered.length,
      hasMore: end < filtered.length,
      page
    };
  };

  return {
    getFavorites,
    toggleFavorite,
    isFavorite,
    loadVocabulary,
    filterVocabulary,
    getPaginatedVocabulary,
    BATCH_SIZE
  };
}

export { BATCH_SIZE };
