import { NoteModel } from './NoteModel.js';

const normalizeText = (value) => (typeof value === 'string' ? value.toLowerCase() : '');

export const NoteQueryService = {
    toPlainContent(note) {
        return NoteModel.getPlainText(note?.content || '');
    },

    sortByCreatedDesc(notes = []) {
        return [...notes].sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));
    },

    filterNotes(notes = [], query = '', selectedTags = []) {
        const safeQuery = query == null ? '' : String(query);
        const normalizedQuery = normalizeText(safeQuery.trim());
        const activeTags = Array.isArray(selectedTags) ? selectedTags : [];

        return this.sortByCreatedDesc(notes).filter((note) => {
            if (!note || note.deleted) return false;

            const matchesQuery = !normalizedQuery || [
                normalizeText(note.title),
                normalizeText(this.toPlainContent(note)),
                normalizeText(note.tag)
            ].some((field) => field.includes(normalizedQuery));

            const matchesTag = activeTags.length === 0 || (note.tag && activeTags.includes(note.tag));
            return matchesQuery && matchesTag;
        });
    },

    toPreviewText(note, maxLength = 120) {
        const text = this.toPlainContent(note);
        if (text.length <= maxLength) return text;
        return `${text.slice(0, maxLength)}...`;
    },

    toDateKey(isoString = '') {
        return String(isoString).split('T')[0];
    }
};

export default NoteQueryService;
