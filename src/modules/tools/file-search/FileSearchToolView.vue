<template>
    <BasePanel class="panel file-search-panel">
        <section class="search-toolbar">
            <div class="search-input-group">
                <BaseInput :value="searchQuery" type="text" placeholder="输入文件名关键词..."
                    @input="searchQuery = $event.target.value" @keyup.enter="handleSearch" />
            </div>
            <div class="search-input-group narrow">
                <BaseInput :value="extensionFilter" type="text" placeholder="扩展名"
                    @input="extensionFilter = $event.target.value" @keyup.enter="handleSearch" />
            </div>
            <div class="search-input-group drive-select">
                <BaseSelect v-if="!isSearching" :value="searchPath" :options="driveOptions"
                    @change="handlePathChange" />
                <span v-else class="searching-label">
                    {{ searchStatusText }}
                </span>
            </div>
            <BaseButton v-if="!isSearching" variant="primary" @click="handleSearch"
                :disabled="!String(searchQuery).trim()">
                <img :src="searchIcon" alt="" class="btn-icon" />
                <span>搜索</span>
            </BaseButton>
            <BaseButton v-else variant="danger" @click="handleStopSearch">
                <img :src="stopIcon" alt="停止" class="btn-icon" />
                <span>停止</span>
            </BaseButton>
        </section>

        <section class="search-progress" v-if="isSearching">
            <div class="progress-info">
                <span class="progress-path">{{ currentPath || '正在扫描...' }}</span>
                <span class="progress-stats">
                    已扫描 {{ filesScanned }} 个文件 | 找到 {{ searchResults.length }} 个结果
                </span>
            </div>
            <div class="progress-bar">
                <div class="progress-fill" :style="{ width: progressWidth }"></div>
            </div>
        </section>

        <section class="result-section">
            <header class="result-header" v-if="hasSearched">
                <h3>
                    搜索结果
                    <span class="result-count" v-if="searchResults.length > 0">
                        共 {{ searchResults.length }} 个匹配项
                    </span>
                </h3>
                <div class="result-header-actions">
                    <div class="sort-options" v-if="searchResults.length > 0">
                        <span>排序：</span>
                        <BaseSelect v-model="sortBy" :options="sortOptions" @change="handleSortChange" size="sm" />
                    </div>
                </div>
            </header>

            <div class="result-container" v-if="searchResults.length > 0">
                <div v-for="(result, index) in sortedResults" :key="`${result.path}-${index}`" class="result-item"
                    @dblclick="handleOpenFile(result)">
                    <div class="result-icon">
                        <span>{{ getFileIcon(result.extension) }}</span>
                    </div>
                    <div class="result-info">
                        <div class="result-name">{{ result.name }}</div>
                        <div class="result-path" v-tooltip:top="result.path">{{ result.path }}</div>
                    </div>
                    <div class="result-meta">
                        <span class="result-size">{{ result.size }}</span>
                        <span class="result-modified">{{ formatDate(result.modified) }}</span>
                    </div>
                    <div class="result-actions">
                        <BaseButton size="sm" v-tooltip:top="'打开'" @click.stop="handleOpenFile(result)">
                            打开
                        </BaseButton>
                        <BaseButton size="sm" v-tooltip:top="'所在文件夹'" @click.stop="handleOpenFolder(result)">
                            文件夹
                        </BaseButton>
                    </div>
                </div>
            </div>

            <div class="empty-state" v-else-if="hasSearched && !isSearching">
                <img :src="emptySearchIcon" alt="" class="empty-icon-img" />
                <span>没有找到匹配的文件</span>
                <small>尝试更换关键词或扩展名</small>
            </div>

            <div class="empty-state" v-else>
                <img :src="emptySearchIcon" alt="" class="empty-icon-img" />
                <span>开始搜索文件</span>
                <small>输入文件名关键词，选择磁盘分区，点击搜索</small>
            </div>
        </section>
    </BasePanel>
</template>

<script>
import searchIcon from '../../../assets/icons/search-20.svg';
import emptySearchIcon from '../../../assets/icons/empty_search_result.svg';
import stopIcon from '../../../assets/icons/stop-solid.svg';

export default {
    name: 'FileSearchToolView',
    data() {
        return {
            searchQuery: '',
            searchPath: '__all__',
            extensionFilter: '',
            driveOptions: [],
            searchResults: [],
            isSearching: false,
            hasSearched: false,
            filesScanned: 0,
            currentPath: '',
            searchStatusText: '',
            searchIcon,
            emptySearchIcon,
            stopIcon,
            currentSearchId: null,
            sortBy: 'name',
            sortOptions: [
                { value: 'name', label: '名称' },
                { value: 'size', label: '大小' },
                { value: 'modified', label: '修改时间' }
            ]
        };
    },
    computed: {
        progressWidth() {
            const maxDisplay = 10000;
            const percentage = Math.min((this.filesScanned / maxDisplay) * 100, 100);
            return percentage + '%';
        },
        sortedResults() {
            return [...this.searchResults].sort((a, b) => {
                switch (this.sortBy) {
                    case 'name':
                        return a.name.localeCompare(b.name);
                    case 'size':
                        return b.sizeBytes - a.sizeBytes;
                    case 'modified':
                        return new Date(b.modified) - new Date(a.modified);
                    default:
                        return 0;
                }
            });
        }
    },
    async mounted() {
        await this.loadDrives();

        if (window.monoFileSearch?.onProgress) {
            window.monoFileSearch.onProgress((progress) => {
                this.filesScanned = progress.filesScanned || 0;
                this.currentPath = progress.currentPath || '';
                this.searchStatusText = `扫描中... ${progress.filesScanned || 0}`;
            });
        }
    },
    beforeDestroy() {
        this.handleStopSearch();
        if (window.monoFileSearch?.removeProgressListener) {
            window.monoFileSearch.removeProgressListener();
        }
    },
    methods: {
        async loadDrives() {
            const searchApi = window.monoFileSearch;

            // 始终将"全盘"作为第一个选项
            const baseOptions = [{ value: '__all__', label: '全盘' }];

            if (!searchApi?.getDrives) {
                // 模拟系统磁盘
                const mockDrives = [
                    { value: 'C:\\', label: 'C:\\ (系统)' },
                    { value: 'D:\\', label: 'D:\\ (数据)' }
                ];
                this.driveOptions = [...baseOptions, ...mockDrives];
                this.searchPath = '__all__';
                return;
            }

            try {
                const drives = await searchApi.getDrives();
                if (drives && drives.length > 0) {
                    const driveOptions = drives.map(d => ({
                        value: d.name,
                        label: d.label || d.name
                    }));
                    this.driveOptions = [...baseOptions, ...driveOptions];
                    this.searchPath = '__all__';
                } else {
                    // 没有检测到磁盘时，只显示"全盘搜索"
                    this.driveOptions = baseOptions;
                    this.searchPath = '__all__';
                }
            } catch (error) {
                console.error('Failed to load drives:', error);
                // 错误时只显示"全盘搜索"
                this.driveOptions = baseOptions;
                this.searchPath = '__all__';
            }
        },

        async handleSearch() {
            if (!String(this.searchQuery).trim()) {
                return;
            }

            const searchApi = window.monoFileSearch;
            if (!searchApi?.search) {
                console.error('File search API not available');
                return;
            }

            this.isSearching = true;
            this.hasSearched = true;
            this.searchResults = [];
            this.filesScanned = 0;
            this.currentPath = '';
            this.searchStatusText = '准备中...';

            try {
                const options = {
                    keyword: String(this.searchQuery).trim(),
                    extension: String(this.extensionFilter).trim().replace(/^\./, ''),
                    maxResults: 1000,
                    recursive: true
                };

                if (this.searchPath === '__all__') {
                    const allResults = [];
                    const drivesToSearch = this.driveOptions.filter(drive => drive.value !== '__all__');

                    if (drivesToSearch.length > 0) {
                        for (const drive of drivesToSearch) {
                            if (!this.isSearching) break;

                            this.searchStatusText = `正在搜索 ${drive.label}...`;

                            try {
                                const result = await searchApi.search(drive.value, options);
                                this.currentSearchId = result.searchId;
                                if (result && result.results) {
                                    allResults.push(...result.results);
                                }
                            } catch (e) {
                                console.error(`Search failed for ${drive.value}:`, e);
                            }
                        }
                    } else {
                        // 如果没有找到磁盘，默认搜索 C 盘
                        this.searchStatusText = '正在搜索 C:\\...';
                        try {
                            const result = await searchApi.search('C:\\', options);
                            this.currentSearchId = result.searchId;
                            if (result && result.results) {
                                allResults.push(...result.results);
                            }
                        } catch (e) {
                            console.error('Search failed for C:\\:', e);
                        }
                    }

                    this.searchResults = allResults.slice(0, options.maxResults);
                } else {
                    const result = await searchApi.search(this.searchPath, options);
                    this.currentSearchId = result.searchId;
                    if (result && result.results) {
                        this.searchResults = result.results;
                    }
                }
            } catch (error) {
                console.error('Search error:', error);
            } finally {
                this.isSearching = false;
                this.searchStatusText = '';
                this.currentSearchId = null;
            }
        },

        async handleStopSearch() {
            if (this.isSearching && this.currentSearchId) {
                const searchApi = window.monoFileSearch;
                if (searchApi?.cancel) {
                    try {
                        await searchApi.cancel(this.currentSearchId);
                    } catch (error) {
                        console.error('Failed to cancel search:', error);
                    }
                }
            }
            this.isSearching = false;
            this.searchStatusText = '';
            this.currentSearchId = null;
        },

        handlePathChange(newPath) {
            this.searchPath = newPath;
        },

        handleSortChange(newSort) {
            this.sortBy = newSort;
        },

        async handleOpenFile(result) {
            const searchApi = window.monoFileSearch;
            if (!searchApi?.openFile) {
                return;
            }

            try {
                await searchApi.openFile(result.path);
            } catch (error) {
                console.error('Failed to open file:', error);
            }
        },

        async handleOpenFolder(result) {
            const searchApi = window.monoFileSearch;
            if (!searchApi?.openInExplorer) {
                return;
            }

            try {
                await searchApi.openInExplorer(result.path);
            } catch (error) {
                console.error('Failed to open folder:', error);
            }
        },

        getFileIcon(extension) {
            const ext = (extension || '').toLowerCase();
            const iconMap = {
                '.txt': '📄', '.md': '📝', '.pdf': '📕',
                '.doc': '📘', '.docx': '📘', '.xls': '📗', '.xlsx': '📗',
                '.ppt': '📙', '.pptx': '📙',
                '.jpg': '🖼️', '.jpeg': '🖼️', '.png': '🖼️', '.gif': '🖼️', '.bmp': '🖼️', '.svg': '🖼️',
                '.mp3': '🎵', '.wav': '🎵', '.mp4': '🎬', '.avi': '🎬', '.mkv': '🎬',
                '.zip': '📦', '.rar': '📦', '.7z': '📦',
                '.js': '📜', '.ts': '📜', '.json': '📜', '.html': '🌐', '.css': '🎨', '.vue': '💚',
                '.py': '🐍', '.java': '☕', '.cpp': '⚙️', '.c': '⚙️', '.h': '⚙️',
                '.exe': '⚡', '.dll': '⚙️', '.iso': '💿', '.img': '💿'
            };
            return iconMap[ext] || '📄';
        },

        formatDate(dateString) {
            if (!dateString) return '';
            const date = new Date(dateString);
            return date.toLocaleDateString();
        }
    }
};
</script>

<style scoped>
.file-search-panel {
    display: flex;
    flex-direction: column;
    height: 100%;
    overflow: hidden;
}

.search-toolbar {
    display: flex;
    gap: 8px;
    padding: 12px 16px;
    border-bottom: 1px solid var(--border-color);
    align-items: center;
}

.search-input-group {
    flex: 1;
}

.search-input-group.narrow {
    flex: 0 0 100px;
}

.search-input-group.drive-select {
    flex: 0 0 auto;
    min-width: 100px;
}

.searching-label {
    display: flex;
    align-items: center;
    padding: 0 12px;
    font-size: 13px;
    color: var(--text-secondary);
    white-space: nowrap;
}

.search-progress {
    padding: 10px 16px;
    background: var(--surface-2);
    border-bottom: 1px solid var(--border-color);
}

.progress-info {
    display: flex;
    justify-content: space-between;
    font-size: 12px;
    margin-bottom: 6px;
}

.progress-path {
    color: var(--text-secondary);
    max-width: 50%;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

.progress-stats {
    color: var(--accent-color);
    font-weight: 500;
}

.progress-bar {
    height: 3px;
    background: var(--border-color);
    border-radius: 2px;
    overflow: hidden;
}

.progress-fill {
    height: 100%;
    background: var(--accent-color);
    transition: width 0.2s ease;
}

.result-section {
    flex: 1;
    overflow-y: auto;
    padding: 0 16px 16px;
}

.result-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 6px 16px;
    position: sticky;
    top: 0;
    background: var(--bg-primary);
    z-index: 1000;
    white-space: nowrap;
    overflow: hidden;
    border-radius: 8px;
    margin-bottom: 8px;
    box-shadow: 0 2px 10px rgba(0, 0, 0, 0.15);
}

.result-header h3 {
    margin: 0;
    font-size: 14px;
    font-weight: 600;
}

.result-header-actions {
    display: flex;
    align-items: center;
    gap: 12px;
}

.result-count {
    font-size: 11px;
    color: var(--text-tertiary);
    margin-left: 8px;
    font-weight: 400;
}

.sort-options {
    display: flex;
    align-items: center;
    gap: 6px;
    font-size: 12px;
    color: var(--text-secondary);
}

.result-container {
    display: flex;
    flex-direction: column;
    gap: 4px;
}

.result-item {
    display: flex;
    align-items: center;
    gap: 12px;
    padding: 12px 14px;
    background: var(--surface-1);
    border-radius: 6px;
    cursor: pointer;
    transition: background 0.15s ease;
}

.result-item:hover {
    background: var(--surface-2);
}

.result-icon {
    width: 36px;
    height: 36px;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 24px;
    flex-shrink: 0;
    margin-top: 2px;
}

.result-info {
    flex: 1;
    min-width: 0;
    display: flex;
    flex-direction: column;
    gap: 4px;
}

.result-name {
    font-weight: 500;
    font-size: 13px;
    color: var(--text-primary);
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
}

.result-path {
    font-size: 11px;
    color: var(--text-secondary);
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
    max-width: 400px;
}

.result-meta {
    flex-shrink: 0;
    text-align: right;
    display: flex;
    flex-direction: column;
    gap: 4px;
    margin-left: 12px;
    min-width: 120px;
}

.result-size {
    font-size: 12px;
    color: var(--text-secondary);
    font-weight: 500;
}

.result-modified {
    font-size: 11px;
    color: var(--text-tertiary);
}

.result-actions {
    display: flex;
    align-items: center;
    gap: 6px;
    opacity: 0;
    transition: opacity 0.15s ease;
    margin-left: 12px;
    padding-left: 12px;
    border-left: 1px solid var(--border-color);
}

.result-item:hover .result-actions {
    opacity: 1;
}

.empty-state {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    padding: 40px 20px;
    color: var(--text-secondary);
    text-align: center;
    user-select: none;
    -webkit-user-select: none;
}

.empty-icon {
    font-size: 40px;
    margin-bottom: 12px;
    opacity: 0.6;
}

.empty-icon-img {
    width: 220px;
    height: 220px;
    opacity: 0.8;
}

.empty-state span {
    font-size: 16px;
    font-weight: bold;
    margin-top: -32px;
    margin-bottom: 6px;
    color: var(--text-muted);
}

.empty-state small {
    font-size: 12px;
    color: var(--text-muted);
}

.btn-icon {
    width: 16px;
    height: 16px;
    margin-right: 6px;
    filter: brightness(0);
}

[data-theme="dark"] .btn-icon {
    filter: brightness(0) invert(1);
}
</style>
