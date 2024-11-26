<script>
    import { extras } from "../stores";

    let r = false;
    async function restart() {
        r = true;
        await fetch("/restart", { method: "POST" });
        while (true) {
            try {
                const res = await fetch("/");
                if (res.ok) break;
            } catch (err) {}
            await new Promise((r) => setTimeout(r, 1000));
        }
        r = false;
    }

    let s = false;
    async function save(event) {
        s = true;
        const formData = new FormData(event.target);
        await fetch("/extras", { method: "POST", body: new URLSearchParams(formData), redirect: 'manual' });
        s = false;
    }
</script>

<div class="form">
    {#if $extras != null }
    <form action="/extras" method="post" id="extras" on:submit|preventDefault={save}>
        <h2><a href="https://espresense.com/configuration/settings#scanning" target="_blank">Scanning</a></h2>
        <p><label>Known BLE mac addresses (no colons, space seperated):<br /><input name="known_macs" bind:value={$extras.known_macs}/></label></p>
        <p><label>Known BLE identity resolving keys, should be 32 hex chars space seperated:<br /><input name="known_irks" bind:value={$extras.known_irks}/></label></p>
        <p><label>Forget beacon if not seen for (in milliseconds):<br /><input type="number" step="1" min="0" max="3000000" name="forget_ms" placeholder="150000" bind:value={$extras.forget_ms}/></label></p>
        <h2><a href="https://espresense.com/configuration/settings#querying" target="_blank">Querying</a></h2>
        <p><label>Query device ids for characteristics (eg. flora:):<br /><input name="query" bind:value={$extras.query}/></label></p>
        <p><label>Requery interval in seconds:<br /><input type="number" step="1" min="30" max="3600" name="requery_ms" placeholder="300" bind:value={$extras.requery_ms}/></label></p>
        <h2><a href="https://espresense.com/configuration/settings#counting" target="_blank">Counting</a></h2>
        <p><label>Include id prefixes (space seperated):<br /><input name="count_ids" bind:value={$extras.count_ids}/></label></p>
        <p><label>Start counting devices less than distance (in meters):<br /><input type="number" step="0.01" min="0" max="100" name="count_enter" placeholder="2.00" bind:value={$extras.count_enter}/></label></p>
        <p><label>Stop counting devices greater than distance (in meters):<br /><input type="number" step="0.01" min="0" max="100" name="count_exit" placeholder="4.00" bind:value={$extras.count_exit}/></label></p>
        <p><label>Include devices with age less than (in ms):<br /><input type="number" step="1" min="0" max="3000000" name="count_ms" placeholder="30000" bind:value={$extras.count_ms}/></label></p>
        <h2><a href="https://espresense.com/configuration/settings#filtering" target="_blank">Filtering</a></h2>
        <p><label>Include only sending these ids to mqtt (eg. apple:iphone10-6 apple:iphone13-2):<br /><input name="include" bind:value={$extras.include}/></label></p>
        <p><label>Exclude sending these ids to mqtt (eg. exp:20 apple:iphone10-6):<br /><input name="exclude" bind:value={$extras.exclude}/></label></p>
        <p><label>Maximum distance to report (in meters):<br /><input type="number" step="0.01" min="0" max="100" name="max_dist" placeholder="16.00" bind:value={$extras.max_dist}/></label></p>
        <p><label>Report early if beacon has moved more than this distance (in meters):<br /><input type="number" step="0.01" min="0" max="10" name="skip_dist" placeholder="0.50" bind:value={$extras.skip_dist}/></label></p>
        <p><label>Skip reporting if message age is less that this (in milliseconds):<br /><input type="number" step="1" min="0" max="3000000" name="skip_ms" placeholder="5000" bind:value={$extras.skip_ms}/></label></p>
        <h2><a href="https://espresense.com/configuration/settings#calibration" target="_blank">Calibration</a></h2>
        <p><label>Rssi expected from a 0dBm transmitter at 1 meter (NOT used for iBeacons or Eddystone):<br /><input type="number" step="1" min="-100" max="100" name="ref_rssi" placeholder="-65" bind:value={$extras.ref_rssi}/></label></p>
        <p><label>Rssi adjustment for receiver (use only if you know this device has a weak antenna):<br /><input type="number" step="1" min="-100" max="100" name="rx_adj_rssi" placeholder="0" bind:value={$extras.rx_adj_rssi}/></label></p>
        <p><label>Factor used to account for absorption, reflection, or diffraction:<br /><input type="number" step="0.01" min="-100" max="100" name="absorption" placeholder="3.50" bind:value={$extras.absorption}/></label></p>
        <p><label>Rssi expected from this tx power at 1m (used for node iBeacon):<br /><input type="number" step="1" min="-100" max="100" name="tx_ref_rssi" placeholder="-59" bind:value={$extras.tx_ref_rssi}/></label></p>
        <h2><a href="https://espresense.com/configuration/settings#leds" target="_blank">LEDs</a></h2>
        <h4>LED 1:</h4>
        <p><label>LED Type:<br /><select name="led_1_type" bind:value={$extras.led_1_type}><option disabled selected hidden>PWM</option><option value="0">PWM</option><option value="1">PWM Inverted</option><option value="2">Addressable GRB</option><option value="3">Addressable GRBW</option><option value="4">Addressable RGB</option><option value="5">Addressable RGBW</option></select></label></p>
        <p><label>Pin (-1 to disable):<br /><input type="number" step="1" min="-1" max="39" name="led_1_pin" placeholder="2" bind:value={$extras.led_1_pin}/></label></p>
        <p><label>Count (only applies to Addressable LEDs):<br /><input type="number" step="1" min="-1" max="39" name="led_1_cnt" placeholder="1" bind:value={$extras.led_1_cnt}/></label></p>
        <p><label>LED Control:<br /><select name="led_1_cntrl" bind:value={$extras.led_1_cntrl}><option disabled selected hidden>Status</option><option value="0">MQTT</option><option value="1">Status</option><option value="2">Motion</option><option value="3">Count</option></select></label></p>
        <h4>LED 2:</h4>
        <p><label>LED Type:<br /><select name="led_2_type" bind:value={$extras.led_2_type}><option disabled selected hidden>PWM</option><option value="0">PWM</option><option value="1">PWM Inverted</option><option value="2">Addressable GRB</option><option value="3">Addressable GRBW</option><option value="4">Addressable RGB</option><option value="5">Addressable RGBW</option></select></label></p>
        <p><label>Pin (-1 to disable):<br /><input type="number" step="1" min="-1" max="39" name="led_2_pin" placeholder="-1" bind:value={$extras.led_2_pin}/></label></p>
        <p><label>Count (only applies to Addressable LEDs):<br /><input type="number" step="1" min="-1" max="39" name="led_2_cnt" placeholder="1" bind:value={$extras.led_2_cnt}/></label></p>
        <p><label>LED Control:<br /><select name="led_2_cntrl" bind:value={$extras.led_2_cntrl}><option disabled selected hidden>MQTT</option><option value="0">MQTT</option><option value="1">Status</option><option value="2">Motion</option><option value="3">Count</option></select></label></p>
        <h4>LED 3:</h4>
        <p><label>LED Type:<br /><select name="led_3_type" bind:value={$extras.led_3_type}><option disabled selected hidden>PWM</option><option value="0">PWM</option><option value="1">PWM Inverted</option><option value="2">Addressable GRB</option><option value="3">Addressable GRBW</option><option value="4">Addressable RGB</option><option value="5">Addressable RGBW</option></select></label></p>
        <p><label>Pin (-1 to disable):<br /><input type="number" step="1" min="-1" max="39" name="led_3_pin" placeholder="-1" bind:value={$extras.led_3_pin}/></label></p>
        <p><label>Count (only applies to Addressable LEDs):<br /><input type="number" step="1" min="-1" max="39" name="led_3_cnt" placeholder="1" bind:value={$extras.led_3_cnt}/></label></p>
        <p><label>LED Control:<br /><select name="led_3_cntrl" bind:value={$extras.led_3_cntrl}><option disabled selected hidden>MQTT</option><option value="0">MQTT</option><option value="1">Status</option><option value="2">Motion</option><option value="3">Count</option></select></label></p>
        <h2><a href="https://espresense.com/configuration/settings#gpio-sensors" target="_blank">GPIO Sensors</a></h2>
        <h4>PIR:</h4>
        <p><label>PIR motion pin type:<br /><select name="pir_type" bind:value={$extras.pir_type}><option disabled selected hidden>Pullup</option><option value="0">Pullup</option><option value="1">Pullup Inverted</option><option value="2">Pulldown</option><option value="3">Pulldown Inverted</option><option value="4">Floating</option><option value="5">Floating Inverted</option></select></label></p>
        <p><label>PIR motion pin (-1 for disable):<br /><input type="number" step="1" name="pir_pin" placeholder="-1" bind:value={$extras.pir_pin}/></label></p>
        <p><label>PIR motion timeout (in seconds):<br /><input type="number" step="0.01" min="0" max="300" name="pir_timeout" placeholder="0.50" bind:value={$extras.pir_timeout}/></label></p>
        <h4>Radar:</h4>
        <p><label>Radar motion pin type:<br /><select name="radar_type" bind:value={$extras.radar_type}><option disabled selected hidden>Pullup</option><option value="0">Pullup</option><option value="1">Pullup Inverted</option><option value="2">Pulldown</option><option value="3">Pulldown Inverted</option><option value="4">Floating</option><option value="5">Floating Inverted</option></select></label></p>
        <p><label>Radar motion pin (-1 for disable):<br /><input type="number" step="1" name="radar_pin" placeholder="-1" bind:value={$extras.radar_pin}/></label></p>
        <p><label>Radar motion timeout (in seconds):<br /><input type="number" step="0.01" min="0" max="300" name="radar_timeout" placeholder="0.50" bind:value={$extras.radar_timeout}/></label></p>
        <h4>Switch One:</h4>
        <p><label>Switch One pin type:<br /><select name="switch_1_type" bind:value={$extras.switch_1_type}><option disabled selected hidden>Pullup</option><option value="0">Pullup</option><option value="1">Pullup Inverted</option><option value="2">Pulldown</option><option value="3">Pulldown Inverted</option><option value="4">Floating</option><option value="5">Floating Inverted</option></select></label></p>
        <p><label>Switch One pin (-1 for disable):<br /><input type="number" step="1" name="switch_1_pin" placeholder="-1" bind:value={$extras.switch_1_pin}/></label></p>
        <p><label>Switch One timeout (in seconds):<br /><input type="number" step="0.01" min="0" max="300" name="switch_1_timeout" placeholder="0.50" bind:value={$extras.switch_1_timeout}/></label></p>
        <h4>Switch Two:</h4>
        <p><label>Switch Two pin type:<br /><select name="switch_2_type" bind:value={$extras.switch_2_type}><option disabled selected hidden>Pullup</option><option value="0">Pullup</option><option value="1">Pullup Inverted</option><option value="2">Pulldown</option><option value="3">Pulldown Inverted</option><option value="4">Floating</option><option value="5">Floating Inverted</option></select></label></p>
        <p><label>Switch Two pin (-1 for disable):<br /><input type="number" step="1" name="switch_2_pin" placeholder="-1" bind:value={$extras.switch_2_pin}/></label></p>
        <p><label>Switch Two timeout (in seconds):<br /><input type="number" step="0.01" min="0" max="300" name="switch_2_timeout" placeholder="0.50" bind:value={$extras.switch_2_timeout}/></label></p>
        <h4>Button One:</h4>
        <p><label>Button One pin type:<br /><select name="button_1_type" bind:value={$extras.button_1_type}><option disabled selected hidden>Pullup</option><option value="0">Pullup</option><option value="1">Pullup Inverted</option><option value="2">Pulldown</option><option value="3">Pulldown Inverted</option><option value="4">Floating</option><option value="5">Floating Inverted</option></select></label></p>
        <p><label>Button One pin (-1 for disable):<br /><input type="number" step="1" name="button_1_pin" placeholder="-1" bind:value={$extras.button_1_pin}/></label></p>
        <p><label>Button One timeout (in seconds):<br /><input type="number" step="0.01" min="0" max="300" name="button_1_timeout" placeholder="0.50" bind:value={$extras.button_1_timeout}/></label></p>
        <h4>Button Two:</h4>
        <p><label>Button Two pin type:<br /><select name="button_2_type" bind:value={$extras.button_2_type}><option disabled selected hidden>Pullup</option><option value="0">Pullup</option><option value="1">Pullup Inverted</option><option value="2">Pulldown</option><option value="3">Pulldown Inverted</option><option value="4">Floating</option><option value="5">Floating Inverted</option></select></label></p>
        <p><label>Button Two pin (-1 for disable):<br /><input type="number" step="1" name="button_2_pin" placeholder="-1" bind:value={$extras.button_2_pin}/></label></p>
        <p><label>Button Two timeout (in seconds):<br /><input type="number" step="0.01" min="0" max="300" name="button_2_timeout" placeholder="0.50" bind:value={$extras.button_2_timeout}/></label></p>
        <h4>DHT:</h4>
        <p><label>DHT11 sensor pin (-1 for disable):<br /><input type="number" step="1" name="dht11_pin" placeholder="-1" bind:value={$extras.dht11_pin}/></label></p>
        <p><label>DHT22 sensor pin (-1 for disable):<br /><input type="number" step="1" name="dht22_pin" placeholder="-1" bind:value={$extras.dht22_pin}/></label></p>
        <p><label>DHT temperature offset:<br /><input type="number" step="0.01" min="-40" max="125" name="dhtTemp_offset" placeholder="0.00" bind:value={$extras.dhtTemp_offset}/></label></p>
        <h2><a href="https://espresense.com/configuration/settings#i2c-settings" target="_blank">I2C Settings</a></h2>
        <h4>Bus 1:</h4>
        <p><label>SDA pin (-1 to disable):<br /><input type="number" step="1" min="-1" max="39" name="I2C_Bus_1_SDA" placeholder="21" bind:value={$extras.I2C_Bus_1_SDA}/></label></p>
        <p><label>SCL pin (-1 to disable):<br /><input type="number" step="1" min="-1" max="39" name="I2C_Bus_1_SCL" placeholder="22" bind:value={$extras.I2C_Bus_1_SCL}/></label></p>
        <h4>Bus 2:</h4>
        <p><label>SDA pin (-1 to disable):<br /><input type="number" step="1" min="-1" max="39" name="I2C_Bus_2_SDA" placeholder="-1" bind:value={$extras.I2C_Bus_2_SDA}/></label></p>
        <p><label>SCL pin (-1 to disable):<br /><input type="number" step="1" min="-1" max="39" name="I2C_Bus_2_SCL" placeholder="-1" bind:value={$extras.I2C_Bus_2_SCL}/></label></p>
        <p><label class="c"><input type="checkbox" name="I2CDebug" bind:value={$extras.I2CDebug}/> Debug I2C addreses. Look at the serial log to get the correct address (default: &#x2610;)</label></p>
        <h2><a href="https://espresense.com/configuration/settings#i2c-sensors" target="_blank">I2C Sensors</a></h2>
        <h4>AHTX0 - Temperature + Humidity Sensor:</h4>
        <p><label>I2C Bus:<br /><input type="number" step="1" min="1" max="2" name="AHTX0_I2c_Bus" placeholder="1" bind:value={$extras.AHTX0_I2c_Bus}/></label></p>
        <p><label>I2C address (0x38 or 0x39):<br /><input name="AHTX0_I2c" bind:value={$extras.AHTX0_I2c}/></label></p>
        <h4>BH1750 - Ambient Light Sensor:</h4>
        <p><label>I2C Bus:<br /><input type="number" step="1" min="1" max="2" name="BH1750_I2c_Bus" placeholder="1" bind:value={$extras.BH1750_I2c_Bus}/></label></p>
        <p><label>I2C address (0x23 or 0x5C):<br /><input name="BH1750_I2c" bind:value={$extras.BH1750_I2c}/></label></p>
        <h4>BME280 - Humidity + Temp + Pressure Sensor:</h4>
        <p><label>I2C Bus:<br /><input type="number" step="1" min="1" max="2" name="BME280_I2c_Bus" placeholder="1" bind:value={$extras.BME280_I2c_Bus}/></label></p>
        <p><label>I2C address (0x76 or 0x77):<br /><input name="BME280_I2c" bind:value={$extras.BME280_I2c}/></label></p>
        <h4>BMP085/BMP180 - Barometric Pressure + Temperature:</h4>
        <p><label>I2C Bus:<br /><input type="number" step="1" min="1" max="2" name="BMP180_I2c_Bus" placeholder="1" bind:value={$extras.BMP180_I2c_Bus}/></label></p>
        <p><label>I2C address (0x77):<br /><input name="BMP180_I2c" bind:value={$extras.BMP180_I2c}/></label></p>
        <h4>BMP280 - Barometric Pressure + Temperature Sensor:</h4>
        <p><label>I2C Bus:<br /><input type="number" step="1" min="1" max="2" name="BMP280_I2c_Bus" placeholder="1" bind:value={$extras.BMP280_I2c_Bus}/></label></p>
        <p><label>I2C address (0x76 or 0x77):<br /><input name="BMP280_I2c" bind:value={$extras.BMP280_I2c}/></label></p>
        <h4>SHTC1/3, SHTW1/2, SHT3x/4x, SHT85 - Temperature and Humidity Sensor:</h4>
        <p><label>I2C Bus (-1 to disable):<br /><input type="number" step="1" min="1" max="2" name="SHT_I2c_Bus" placeholder="-1" bind:value={$extras.SHT_I2c_Bus}/></label></p>
        <h4>TSL2561 - Ambient Light Sensor:</h4>
        <p><label>I2C Bus:<br /><input type="number" step="1" min="1" max="2" name="TSL2561_I2c_Bus" placeholder="1" bind:value={$extras.TSL2561_I2c_Bus}/></label></p>
        <p><label>I2C address (0x39, 0x49 or 0x29):<br /><input name="TSL2561_I2c" bind:value={$extras.TSL2561_I2c}/></label></p>
        <p><label>Gain (auto, 1x or 16x):<br /><input name="TSL2561_I2c_Gain" placeholder="auto" bind:value={$extras.TSL2561_I2c_Gain}/></label></p>
        <h4>SGP30 - Air Quality Sensor:</h4>
        <p><label>I2C Bus:<br /><input type="number" step="1" min="1" max="2" name="SGP30_I2c_Bus" placeholder="1" bind:value={$extras.SGP30_I2c_Bus}/></label></p>
        <p><label>I2C address (0x58):<br /><input name="SGP30_I2c" bind:value={$extras.SGP30_I2c}/></label></p>
        <h4>HX711 - Weight Sensor:</h4>
        <p><label>HX711 SCK (Clock) pin:<br /><input type="number" step="1" name="HX711_sckPin" placeholder="0" bind:value={$extras.HX711_sckPin}/></label></p>
        <p><label>HX711 DOUT (Data) pin:<br /><input type="number" step="1" name="HX711_doutPin" placeholder="0" bind:value={$extras.HX711_doutPin}/></label></p>
        <h4>DS18B20:</h4>
        <p><label>DS18B20 sensor pin (-1 for disable):<br /><input type="number" step="1" name="ds18b20_pin" placeholder="-1" bind:value={$extras.ds18b20_pin}/></label></p>
        <p><label>DS18B20 temperature offset:<br /><input type="number" step="0.01" min="-40" max="125" name="dsTemp_offset" placeholder="0.00" bind:value={$extras.dsTemp_offset}/></label></p>
        <h4>ENVIV - Barometric Pressure + Temperature Sensor:</h4>
        <p><label>I2C Bus:<br /><input type="number" step="1" min="1" max="2" name="ENVIVBMP280_I2c_Bus" placeholder="1" bind:value={$extras.ENVIVBMP280_I2c_Bus}/></label></p>
        <p><label>I2C address (0x76 or 0x77):<br /><input name="ENVIVBMP280_I2c" bind:value={$extras.ENVIVBMP280_I2c}/></label></p>
        <div class="bc"><button class="btn left" on:click|preventDefault={restart}>{r ? "Restarting..." : "Restart"}</button><input class="btn right" type="submit" value={s ? "Saving..." : "Save"} /></div>
    </form>
    {/if}
</div>

<style>
    .form {
        padding: 1em;
        background: #ccc;
        max-width: 40em;
        margin: 0;
        position: relative;
    }
    * {
        box-sizing: border-box;
    }

    h2 {
        margin-top: 1em;
        margin-bottom: 1em;
        font: 1.5em sans-serif;
    }
    h4 {
        margin-top: 1em;
        margin-bottom: 1em;
        font: 1.25em sans-serif;
    }
    a:link {
        color: #000;
        text-decoration: none;
    }
    label {
        clear: both;
    }
    select,
    input:not([type^="c"]) {
        display: block;
        width: 100%;
        border: 1px solid #444;
        padding: 0.3ex;
    }
    input[type^="c"] {
        float: left;
        margin-top: 0.5em;
        margin-left: -1.5em;
    }
    :not([type^="s"]):focus {
        outline: 2px solid #de1;
    }
    p::before {
        margin-left: -2em;
        float: left;
        padding-top: 1ex;
    }
    .c {
        display: block;
        padding-left: 2em;
    }
    .bc {
        position: sticky;
        bottom: 0;
        width: 100%;
        display: flex;
        justify-content: space-between;
        margin-top: 1em;
    }
    .btn {
        background: #de1;
        font-size: 1.5em;
        width: 8em !important;
        padding: 0.5em !important;
        border: 1px solid #000;
        border-radius: 1ex;
        justify-content: space-between;
    }
    .left {
        left: 0;
    }
    .right {
        right: 0;
    }
</style>
