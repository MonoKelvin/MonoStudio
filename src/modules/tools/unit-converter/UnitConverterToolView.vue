<template>
    <div class="unit-converter">
        <div class="converter-header">
            <div class="category-tabs" ref="tabsRef" :style="tabsStyle">
                <div class="tab-indicator"></div>
                <button
                    v-for="cat in categories"
                    :key="cat.value"
                    class="category-tab"
                    :class="{ active: selectedCategory === cat.value }"
                    @click="selectCategory(cat.value)"
                >
                    {{ cat.label }}
                </button>
            </div>
        </div>

        <div class="converter-content">
            <div class="conversion-card">
                <div class="conversion-panel from-panel">
                    <span class="panel-label">从</span>
                    <div class="panel-controls">
                        <BaseSpinbox
                            v-model="inputValue"
                            :decimal="true"
                            class="value-spinbox"
                        />
                        <BaseSelect
                            v-model="fromUnit"
                            :options="currentUnits"
                            class="unit-select"
                            @change="convert"
                        />
                    </div>
                </div>

                <button class="swap-btn" @click="swapUnits" title="交换单位">
                    <svg class="swap-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <path d="M8 7L4 12L8 17"/>
                        <path d="M16 7L20 12L16 17"/>
                        <path d="M4 12H20"/>
                    </svg>
                </button>

                <div class="conversion-panel to-panel">
                    <span class="panel-label">到</span>
                    <div class="panel-controls">
                        <BaseInput
                            :model-value="resultValue"
                            type="text"
                            class="result-input"
                            readonly
                        />
                        <BaseSelect
                            v-model="toUnit"
                            :options="currentUnits"
                            class="unit-select"
                            @change="convert"
                        />
                    </div>
                </div>
            </div>

            <div class="presets-card" v-if="currentPresets.length > 0">
                <div class="presets-header">
                    <span class="presets-title">常用转换</span>
                </div>
                <div class="presets-grid">
                    <button
                        v-for="preset in currentPresets"
                        :key="preset.from + preset.to"
                        class="preset-btn"
                        @click="applyPreset(preset)"
                    >
                        <span class="preset-from">{{ preset.from }}</span>
                        <svg class="preset-arrow" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                            <path d="M5 12h14M12 5l7 7-7 7"/>
                        </svg>
                        <span class="preset-to">{{ preset.to }}</span>
                    </button>
                </div>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, computed, watch, nextTick, onMounted, onUnmounted } from 'vue';
import BaseSpinbox from '../../../components/base/BaseSpinbox.vue';
import BaseSelect from '../../../components/base/BaseSelect.vue';

const tabsRef = ref(null);
const selectedCategory = ref('length');
const inputValue = ref(1);
const fromUnit = ref('meter');
const toUnit = ref('kilometer');

const indicatorLeft = ref(0);
const indicatorWidth = ref(0);

const tabsStyle = computed(() => ({
    '--indicator-left': indicatorLeft.value + 'px',
    '--indicator-width': indicatorWidth.value + 'px'
}));

const updateIndicator = () => {
    if (!tabsRef.value) return;

    const tabs = tabsRef.value.querySelectorAll('.category-tab');
    const activeIndex = Array.from(tabs).findIndex(
        tab => tab.classList.contains('active')
    );

    if (activeIndex === -1) return;

    const activeTab = tabs[activeIndex];
    const tabRect = activeTab.getBoundingClientRect();
    const containerRect = tabsRef.value.getBoundingClientRect();

    indicatorLeft.value = tabRect.left - containerRect.left;
    indicatorWidth.value = tabRect.width;
};

let resizeTimer = null;

const handleResize = () => {
    clearTimeout(resizeTimer);
    resizeTimer = setTimeout(updateIndicator, 50);
};

onMounted(() => {
    nextTick(() => {
        updateIndicator();
    });
    window.addEventListener('resize', handleResize);
});

onUnmounted(() => {
    window.removeEventListener('resize', handleResize);
    clearTimeout(resizeTimer);
});

const categories = [
    { value: 'length', label: '长度' },
    { value: 'weight', label: '重量' },
    { value: 'temperature', label: '温度' },
    { value: 'area', label: '面积' },
    { value: 'volume', label: '体积' },
    { value: 'time', label: '时间' },
    { value: 'speed', label: '速度' },
    { value: 'data', label: '数据' },
    { value: 'pressure', label: '压强' },
    { value: 'energy', label: '能量' }
];

const unitData = {
    length: {
        units: [
            { label: '米 (m)', value: 'meter' },
            { label: '千米 (km)', value: 'kilometer' },
            { label: '厘米 (cm)', value: 'centimeter' },
            { label: '毫米 (mm)', value: 'millimeter' },
            { label: '微米 (μm)', value: 'micrometer' },
            { label: '纳米 (nm)', value: 'nanometer' },
            { label: '英寸 (in)', value: 'inch' },
            { label: '英尺 (ft)', value: 'foot' },
            { label: '码 (yd)', value: 'yard' },
            { label: '英里 (mi)', value: 'mile' }
        ],
        toBase: {
            meter: 1,
            kilometer: 1000,
            centimeter: 0.01,
            millimeter: 0.001,
            micrometer: 0.000001,
            nanometer: 0.000000001,
            inch: 0.0254,
            foot: 0.3048,
            yard: 0.9144,
            mile: 1609.344
        },
        presets: [
            { from: '1 km', to: '1000 m' },
            { from: '1 ft', to: '30.48 cm' },
            { from: '1 in', to: '2.54 cm' },
            { from: '1 mi', to: '1609.344 m' }
        ]
    },
    weight: {
        units: [
            { label: '千克 (kg)', value: 'kilogram' },
            { label: '克 (g)', value: 'gram' },
            { label: '毫克 (mg)', value: 'milligram' },
            { label: '微克 (μg)', value: 'microgram' },
            { label: '吨 (t)', value: 'ton' },
            { label: '磅 (lb)', value: 'pound' },
            { label: '盎司 (oz)', value: 'ounce' },
            { label: '克拉 (ct)', value: 'carat' }
        ],
        toBase: {
            kilogram: 1,
            gram: 0.001,
            milligram: 0.000001,
            microgram: 0.000000001,
            ton: 1000,
            pound: 0.45359237,
            ounce: 0.0283495231,
            carat: 0.0002
        },
        presets: [
            { from: '1 kg', to: '1000 g' },
            { from: '1 lb', to: '453.592 g' },
            { from: '1 oz', to: '28.35 g' },
            { from: '1 t', to: '1000 kg' }
        ]
    },
    temperature: {
        units: [
            { label: '摄氏度 (°C)', value: 'celsius' },
            { label: '华氏度 (°F)', value: 'fahrenheit' },
            { label: '开尔文 (K)', value: 'kelvin' }
        ],
        presets: [
            { from: '0°C', to: '32°F' },
            { from: '100°C', to: '212°F' },
            { from: '-40°C', to: '-40°F' }
        ]
    },
    area: {
        units: [
            { label: '平方米 (m²)', value: 'square_meter' },
            { label: '平方千米 (km²)', value: 'square_kilometer' },
            { label: '平方厘米 (cm²)', value: 'square_centimeter' },
            { label: '公顷 (ha)', value: 'hectare' },
            { label: '亩 (mu)', value: 'mu' },
            { label: '平方英寸 (in²)', value: 'square_inch' },
            { label: '平方英尺 (ft²)', value: 'square_foot' },
            { label: '平方码 (yd²)', value: 'square_yard' },
            { label: '英亩 (ac)', value: 'acre' }
        ],
        toBase: {
            square_meter: 1,
            square_kilometer: 1000000,
            square_centimeter: 0.0001,
            hectare: 10000,
            mu: 666.6667,
            square_inch: 0.00064516,
            square_foot: 0.092903,
            square_yard: 0.836127,
            acre: 4046.8564224
        },
        presets: [
            { from: '1 ha', to: '10000 m²' },
            { from: '1 km²', to: '100 ha' },
            { from: '1 ac', to: '4046.86 m²' },
            { from: '1 mu', to: '666.67 m²' }
        ]
    },
    volume: {
        units: [
            { label: '立方米 (m³)', value: 'cubic_meter' },
            { label: '升 (L)', value: 'liter' },
            { label: '毫升 (mL)', value: 'milliliter' },
            { label: '立方厘米 (cm³)', value: 'cubic_centimeter' },
            { label: '立方英寸 (in³)', value: 'cubic_inch' },
            { label: '立方英尺 (ft³)', value: 'cubic_foot' },
            { label: '加仑 (gal)', value: 'gallon' },
            { label: '夸脱 (qt)', value: 'quart' }
        ],
        toBase: {
            cubic_meter: 1,
            liter: 0.001,
            milliliter: 0.000001,
            cubic_centimeter: 0.000001,
            cubic_inch: 0.0000163871,
            cubic_foot: 0.0283168,
            gallon: 0.00378541,
            quart: 0.000946353
        },
        presets: [
            { from: '1 L', to: '1000 mL' },
            { from: '1 gal', to: '3.785 L' },
            { from: '1 ft³', to: '28.32 L' },
            { from: '1 m³', to: '1000 L' }
        ]
    },
    time: {
        units: [
            { label: '秒 (s)', value: 'second' },
            { label: '毫秒 (ms)', value: 'millisecond' },
            { label: '微秒 (μs)', value: 'microsecond' },
            { label: '分钟 (min)', value: 'minute' },
            { label: '小时 (h)', value: 'hour' },
            { label: '天 (d)', value: 'day' },
            { label: '周 (wk)', value: 'week' },
            { label: '月 (mo)', value: 'month' },
            { label: '年 (yr)', value: 'year' }
        ],
        toBase: {
            second: 1,
            millisecond: 0.001,
            microsecond: 0.000001,
            minute: 60,
            hour: 3600,
            day: 86400,
            week: 604800,
            month: 2629746,
            year: 31556952
        },
        presets: [
            { from: '1 h', to: '3600 s' },
            { from: '1 d', to: '86400 s' },
            { from: '1 wk', to: '7 d' },
            { from: '1 yr', to: '365.25 d' }
        ]
    },
    speed: {
        units: [
            { label: '米/秒 (m/s)', value: 'meter_per_second' },
            { label: '千米/时 (km/h)', value: 'kilometer_per_hour' },
            { label: '英里/时 (mph)', value: 'mile_per_hour' },
            { label: '节 (kn)', value: 'knot' },
            { label: '英尺/秒 (ft/s)', value: 'foot_per_second' },
            { label: '马赫 (Ma)', value: 'mach' }
        ],
        toBase: {
            meter_per_second: 1,
            kilometer_per_hour: 0.277778,
            mile_per_hour: 0.44704,
            knot: 0.514444,
            foot_per_second: 0.3048,
            mach: 343
        },
        presets: [
            { from: '1 km/h', to: '0.278 m/s' },
            { from: '1 mph', to: '1.609 km/h' },
            { from: '1 Ma', to: '1235 km/h' },
            { from: '1 kn', to: '1.852 km/h' }
        ]
    },
    data: {
        units: [
            { label: '字节 (B)', value: 'byte' },
            { label: '千字节 (KB)', value: 'kilobyte' },
            { label: '兆字节 (MB)', value: 'megabyte' },
            { label: '吉字节 (GB)', value: 'gigabyte' },
            { label: '太字节 (TB)', value: 'terabyte' },
            { label: '位 (bit)', value: 'bit' },
            { label: '千位 (Kbit)', value: 'kilobit' },
            { label: '兆位 (Mbit)', value: 'megabit' }
        ],
        toBase: {
            byte: 1,
            kilobyte: 1024,
            megabyte: 1048576,
            gigabyte: 1073741824,
            terabyte: 1099511627776,
            bit: 0.125,
            kilobit: 128,
            megabit: 131072
        },
        presets: [
            { from: '1 KB', to: '1024 B' },
            { from: '1 MB', to: '1024 KB' },
            { from: '1 GB', to: '1024 MB' },
            { from: '1 B', to: '8 bit' }
        ]
    },
    pressure: {
        units: [
            { label: '帕斯卡 (Pa)', value: 'pascal' },
            { label: '千帕 (kPa)', value: 'kilopascal' },
            { label: '兆帕 (MPa)', value: 'megapascal' },
            { label: '巴 (bar)', value: 'bar' },
            { label: '标准大气压 (atm)', value: 'atmosphere' },
            { label: '毫米汞柱 (mmHg)', value: 'mmhg' },
            { label: '磅力/平方英寸 (psi)', value: 'psi' }
        ],
        toBase: {
            pascal: 1,
            kilopascal: 1000,
            megapascal: 1000000,
            bar: 100000,
            atmosphere: 101325,
            mmhg: 133.322,
            psi: 6894.76
        },
        presets: [
            { from: '1 atm', to: '101.325 kPa' },
            { from: '1 bar', to: '100 kPa' },
            { from: '1 psi', to: '6.895 kPa' },
            { from: '1 MPa', to: '10 bar' }
        ]
    },
    energy: {
        units: [
            { label: '焦耳 (J)', value: 'joule' },
            { label: '千焦 (kJ)', value: 'kilojoule' },
            { label: '卡路里 (cal)', value: 'calorie' },
            { label: '千卡 (kcal)', value: 'kilocalorie' },
            { label: '瓦时 (Wh)', value: 'watthour' },
            { label: '千瓦时 (kWh)', value: 'kilowatthour' },
            { label: '电子伏特 (eV)', value: 'electronvolt' },
            { label: '英热单位 (BTU)', value: 'btu' }
        ],
        toBase: {
            joule: 1,
            kilojoule: 1000,
            calorie: 4.184,
            kilocalorie: 4184,
            watthour: 3600,
            kilowatthour: 3600000,
            electronvolt: 1.60218e-19,
            btu: 1055.06
        },
        presets: [
            { from: '1 kcal', to: '4184 J' },
            { from: '1 kWh', to: '3600 kJ' },
            { from: '1 cal', to: '4.184 J' },
            { from: '1 BTU', to: '1055 J' }
        ]
    }
};

const currentUnits = computed(() => {
    return unitData[selectedCategory.value]?.units || [];
});

const currentPresets = computed(() => {
    return unitData[selectedCategory.value]?.presets || [];
});

const resultValue = ref('');

const convert = () => {
    if (inputValue.value === '' || inputValue.value === null || isNaN(inputValue.value)) {
        resultValue.value = '';
        return;
    }

    const value = inputValue.value;

    if (selectedCategory.value === 'temperature') {
        resultValue.value = convertTemperature(value, fromUnit.value, toUnit.value);
    } else {
        const categoryData = unitData[selectedCategory.value];
        const toBase = categoryData.toBase;
        const baseValue = value * toBase[fromUnit.value];
        const result = baseValue / toBase[toUnit.value];
        resultValue.value = formatResult(result);
    }
};

const convertTemperature = (value, from, to) => {
    let celsius;

    switch (from) {
        case 'celsius':
            celsius = value;
            break;
        case 'fahrenheit':
            celsius = (value - 32) * 5 / 9;
            break;
        case 'kelvin':
            celsius = value - 273.15;
            break;
        default:
            celsius = value;
    }

    let result;
    switch (to) {
        case 'celsius':
            result = celsius;
            break;
        case 'fahrenheit':
            result = celsius * 9 / 5 + 32;
            break;
        case 'kelvin':
            result = celsius + 273.15;
            break;
        default:
            result = celsius;
    }

    return formatResult(result);
};

const formatResult = (value) => {
    if (Math.abs(value) < 0.0001 || Math.abs(value) >= 10000000) {
        return value.toExponential(4);
    }

    const rounded = Math.round(value * 10000) / 10000;

    if (Number.isInteger(rounded)) {
        return rounded.toString();
    }

    return rounded.toString();
};

const selectCategory = (category) => {
    selectedCategory.value = category;
    const units = currentUnits.value;
    fromUnit.value = units[0]?.value || '';
    toUnit.value = units[1]?.value || units[0]?.value || '';
    convert();
};

const swapUnits = () => {
    const temp = fromUnit.value;
    fromUnit.value = toUnit.value;
    toUnit.value = temp;
    convert();
};

const applyPreset = (preset) => {
    const match = preset.from.match(/^([\d.]+)\s*(\w+)$/);
    if (match) {
        inputValue.value = parseFloat(match[1]);
        const unitLabel = match[2];
        const unit = currentUnits.value.find(u => u.label.includes(unitLabel));
        if (unit) {
            fromUnit.value = unit.value;
        }
        convert();
    }
};

watch(selectedCategory, async () => {
    convert();
    await nextTick();
    updateIndicator();
});

watch([fromUnit, toUnit, inputValue], () => {
    convert();
});
</script>

<style scoped>
.unit-converter {
    padding: var(--spacing-md);
    height: 100%;
    max-height: 100%;
    overflow-y: auto;
    box-sizing: border-box;
    display: flex;
    flex-direction: column;
    gap: var(--spacing-md);
}

.converter-header {
    flex-shrink: 0;
}

.converter-title {
    font-size: var(--font-size-lg);
    font-weight: 600;
    color: var(--text-primary);
    margin: 0 0 var(--spacing-sm) 0;
}

.category-tabs {
    display: flex;
    flex-wrap: wrap;
    gap: 6px;
    padding: 4px;
    background: var(--bg-soft);
    border-radius: var(--radius-md);
    position: relative;
}

.tab-indicator {
    position: absolute;
    top: 4px;
    left: var(--indicator-left, 4px);
    width: var(--indicator-width, auto);
    height: calc(100% - 8px);
    background: var(--accent-primary);
    border-radius: var(--radius-sm);
    transition: left 0.3s cubic-bezier(0.4, 0, 0.2, 1),
                width 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    z-index: 0;
    pointer-events: none;
}

.category-tab {
    padding: 8px 14px;
    border: none;
    background: transparent;
    color: var(--text-muted);
    font-size: var(--font-size-sm);
    cursor: pointer;
    border-radius: var(--radius-sm);
    transition: color 0.2s ease;
    position: relative;
    z-index: 1;
}

.category-tab:hover {
    color: var(--text-secondary);
}

.category-tab.active {
    color: white;
    font-weight: 500;
}

.converter-content {
    flex: 1;
    display: flex;
    flex-direction: column;
    gap: var(--spacing-md);
    min-height: 0;
}

.conversion-card {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: var(--spacing-md);
    background: var(--bg-elevated);
    border-radius: var(--radius-lg);
    padding: var(--spacing-lg);
    box-shadow: var(--shadow-card);
}

.conversion-panel {
    display: flex;
    flex-direction: column;
    gap: 6px;
    flex: 1;
    min-width: 0;
}

.panel-label {
    font-size: var(--font-size-sm);
    font-weight: 500;
    color: var(--text-muted);
}

.panel-controls {
    display: flex;
    align-items: center;
    gap: 6px;
    flex: 1;
}

:deep(.ui-spinbox) {
    flex: 1;
    min-width: 80px;
    height: 32px;
}

:deep(.ui-input) {
    flex: 1;
    min-width: 80px;
    height: 32px;
    font-size: var(--font-size-md);
}

:deep(.ui-select-wrap) {
    height: 32px;
    min-width: 120px;
    flex: 1;
}

:deep(.ui-select-trigger) {
    height: 32px;
    font-size: var(--font-size-sm);
    padding: 0 8px;
    width: 100%;
}

.result-input {
    color: var(--accent-primary);
    font-weight: 600;
}

.swap-btn {
    width: auto;
    height: 32px;
    flex-shrink: 0;
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 4px;
    border: none;
    background: var(--accent-primary);
    color: white;
    border-radius: var(--radius-md);
    cursor: pointer;
    transition: all 0.2s ease;
    margin-top: 24px;
    padding: 0 12px;
}

.swap-btn:hover {
    background: var(--accent-active);
    transform: scale(1.02);
}

.swap-btn:active {
    transform: scale(0.98);
}

.swap-icon {
    width: 16px;
    height: 16px;
}

.presets-card {
    background: var(--bg-elevated);
    border-radius: var(--radius-lg);
    padding: var(--spacing-md);
    box-shadow: var(--shadow-card);
}

.presets-header {
    margin-bottom: var(--spacing-xs);
}

.presets-title {
    font-size: var(--font-size-xs);
    font-weight: 500;
    color: var(--text-muted);
    text-transform: uppercase;
    letter-spacing: 0.5px;
}

.presets-grid {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(150px, 1fr));
    gap: 6px;
}

.preset-btn {
    display: flex;
    align-items: center;
    gap: 6px;
    padding: 8px 12px;
    border: 1px solid var(--border-color);
    background: var(--bg-primary);
    color: var(--text-secondary);
    font-size: var(--font-size-sm);
    border-radius: var(--radius-sm);
    cursor: pointer;
    transition: all 0.2s ease;
}

.preset-btn:hover {
    border-color: var(--accent-primary);
    color: var(--accent-primary);
    background: rgba(59, 130, 246, 0.05);
}

.preset-from,
.preset-to {
    font-weight: 500;
    color: var(--text-primary);
}

.preset-btn:hover .preset-from,
.preset-btn:hover .preset-to {
    color: var(--accent-primary);
}

.preset-arrow {
    width: 12px;
    height: 12px;
    color: var(--text-muted);
    flex-shrink: 0;
}

@media (max-width: 768px) {
    .conversion-card {
        flex-direction: column;
        padding: var(--spacing-md);
        gap: var(--spacing-sm);
    }

    .swap-btn {
        margin-top: 0;
        transform: rotate(90deg);
    }

    .swap-btn:hover {
        transform: rotate(90deg) scale(1.05);
    }

    .panel-controls {
        flex-direction: column;
        width: 100%;
    }

    .presets-grid {
        grid-template-columns: 1fr;
    }
}
</style>
