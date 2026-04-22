/**
 * 工作笔记服务模块
 * 提供笔记存储和管理的统一接口
 */
export { NoteModel } from './NoteModel.js';
export { NoteQueryService } from './NoteQueryService.js';
export { workNotesStorageConfig } from './storage.config.js';
export { noteService, default } from './NoteService.js';