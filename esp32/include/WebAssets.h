#pragma once

#include <Arduino.h>

const char ECOSENSE_WEB_APP[] PROGMEM = R"HTML(
<!doctype html>
<html lang="pt-BR">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>EcoSense ESP32 Server</title>
<style>
:root{font-family:Inter,system-ui,sans-serif;color:#17352a;background:#eef4ef}
*{box-sizing:border-box}body{margin:0}.shell{display:grid;grid-template-columns:220px 1fr;min-height:100vh}
aside{background:#173b2b;color:#fff;padding:22px;position:sticky;top:0;height:100vh}
aside h1{margin:0 0 8px}.muted{opacity:.7;font-size:12px}nav{display:grid;gap:8px;margin-top:28px}
nav button{border:0;background:transparent;color:#dfece4;text-align:left;padding:10px;border-radius:10px;cursor:pointer}
nav button:hover,nav button.active{background:#ffffff1f;color:#fff}
main{padding:24px;max-width:1250px;width:100%}.top{display:flex;justify-content:space-between;gap:16px;align-items:center}
.badge{padding:8px 12px;background:#fff;border-radius:999px;box-shadow:0 4px 18px #183b2b12}
section{display:none}section.active{display:block}.grid{display:grid;grid-template-columns:repeat(4,1fr);gap:14px}
.card,.panel{background:#fff;border:1px solid #dbe7de;border-radius:18px;padding:18px;box-shadow:0 8px 24px #183b2b0d}
.card strong{font-size:28px;display:block;margin-top:10px}.panel{margin-top:16px}.two{display:grid;grid-template-columns:1fr 1fr;gap:16px}
label{display:block;margin:10px 0 5px;font-size:13px}input,button.action{width:100%;padding:10px;border:1px solid #ccd9cf;border-radius:10px}
button.action{background:#2f6f4f;color:#fff;border:0;cursor:pointer;margin-top:10px}
button.secondary{background:#edf3ef;color:#17352a}.status-ok{color:#2f8f57}.status-bad{color:#b64a4a}
table{width:100%;border-collapse:collapse;font-size:13px}th,td{text-align:left;padding:8px;border-bottom:1px solid #edf0ee}
pre{white-space:pre-wrap;background:#12261d;color:#dff4e8;padding:14px;border-radius:12px;max-height:380px;overflow:auto}
canvas{width:100%;height:300px;background:#fbfdfb;border-radius:14px}
small.note{display:block;margin-top:8px;color:#66746c}
@media(max-width:900px){.shell{grid-template-columns:1fr}aside{position:relative;height:auto}.grid{grid-template-columns:repeat(2,1fr)}.two{grid-template-columns:1fr}}
@media(max-width:560px){.grid{grid-template-columns:1fr}main{padding:14px}}
</style>
</head>
<body>
<div class="shell">
<aside>
<h1>EcoSense</h1>
<div class="muted">Servidor embarcado ESP32</div>
<nav id="nav">
<button data-tab="dashboard" class="active">Dashboard</button>
<button data-tab="performance">Performance</button>
<button data-tab="history">Histórico 24h</button>
<button data-tab="config">Configuração</button>
<button data-tab="logs">Logs</button>
<button data-tab="about">Sobre</button>
<button data-tab="changelog">Changelog</button>
<button data-tab="ota">OTA</button>
</nav>
</aside>
<main>
<div class="top">
<div><h2 id="title">Dashboard</h2><div class="muted">O navegador fala diretamente com o ESP32.</div></div>
<div class="badge" id="connection">Carregando...</div>
</div>

<section id="dashboard" class="active">
<div class="grid">
<div class="card">Temperatura<strong id="temperature">--</strong><span>°C</span></div>
<div class="card">Umidade<strong id="humidity">--</strong><span>%</span></div>
<div class="card">Luminosidade<strong id="luminosity">--</strong><span>ADC</span></div>
<div class="card">Qualidade do ar<strong id="air">--</strong><span>ADC</span></div>
</div>
<div class="two">
<div class="panel">
<h3>Dispositivo</h3>
<p>Wi-Fi: <b id="wifi">--</b></p>
<p>RSSI: <b id="rssi">--</b> dBm</p>
<p>IP: <b id="ip">--</b></p>
<p>MQTT: <b id="mqtt">--</b></p>
</div>
<div class="panel">
<h3>Atuadores</h3>
<button class="action" onclick="actuator('fan','ON')">Ventilador ON</button>
<button class="action secondary" onclick="actuator('fan','OFF')">Ventilador OFF</button>
<button class="action" onclick="actuator('exhaust','ON')">Exaustor ON</button>
<button class="action secondary" onclick="actuator('exhaust','OFF')">Exaustor OFF</button>
</div>
</div>
</section>

<section id="performance">
<div class="grid">
<div class="card">CPU estimada<strong id="cpu">--</strong><span>%</span></div>
<div class="card">Heap livre<strong id="heap">--</strong><span> bytes</span></div>
<div class="card">Flash<strong id="flash">--</strong><span> bytes</span></div>
<div class="card">Tasks<strong id="tasksCount">--</strong></div>
</div>
<div class="two">
<div class="panel"><h3>Memória e rede</h3><pre id="memory">--</pre></div>
<div class="panel"><h3>Stack por task</h3><table><thead><tr><th>Task</th><th>High water</th></tr></thead><tbody id="tasks"></tbody></table></div>
</div>
<div class="panel"><h3>Tempo de execução das funções</h3><table><thead><tr><th>Função</th><th>Último µs</th><th>Média µs</th><th>Máx µs</th></tr></thead><tbody id="timings"></tbody></table></div>
<small class="note">CPU é uma estimativa da carga das tasks da aplicação EcoSense; frequência e métricas de memória vêm do ESP32.</small>
</section>

<section id="history">
<div class="panel"><h3>Temperatura — últimas 24h</h3><canvas id="chart" width="900" height="300"></canvas></div>
</section>

<section id="config">
<div class="two">
<div class="panel">
<h3>Leitura de sensores</h3>
<label>Intervalo (ms)</label><input id="sampleMs" type="number" min="5000">
<button class="action" onclick="saveSample()">Salvar intervalo</button>
</div>
<div class="panel">
<h3>MQTT</h3>
<label>Host</label><input id="mqttHost">
<button class="action" onclick="saveMqtt()">Salvar broker</button>
</div>
</div>
<div class="two">
<div class="panel">
<h3>Wi-Fi</h3>
<label>SSID</label><input id="ssid">
<label>Senha</label><input id="password" type="password">
<button class="action" onclick="saveWifi()">Trocar Wi-Fi sem regravar firmware</button>
</div>
<div class="panel">
<h3>Energia</h3>
<label>Light Sleep (segundos)</label><input id="sleepSeconds" type="number" min="1" max="3600" value="10">
<button class="action" onclick="sleepNow()">Acionar Light Sleep</button>
</div>
</div>
</section>

<section id="logs">
<div class="panel"><h3>Últimos logs persistidos em flash</h3><button class="action" onclick="loadLogs()">Atualizar</button><a href="/api/logs/export"><button class="action secondary">Exportar logs</button></a><pre id="logText">--</pre></div>
</section>

<section id="about">
<div class="panel">
<h3>Sobre o EcoSense</h3>
<p><b>Universidade:</b> Pontifícia Universidade Católica do Paraná — PUCPR</p>
<p><b>Curso:</b> Engenharia de Software</p>
<p><b>Disciplina:</b> Performance em Sistemas Ciberfísicos</p>
<p><b>Integrantes:</b> preencher nomes completos antes da entrega</p>
<p><b>Contatos:</b> preencher e-mails antes da entrega</p>
<p><b>GitHub:</b> <a href="https://github.com/rafael-mindahead/EcoSense">rafael-mindahead/EcoSense</a></p>
</div>
</section>

<section id="changelog">
<div class="panel"><h3>Changelog</h3>
<p><b>2.0.0</b> — ESP32 transformado em servidor autônomo com Web, API, performance, logs, configuração, OTA, FreeRTOS e persistência local.</p>
<p><b>1.x</b> — MQTT, backend PHP, PostgreSQL, Services e dashboard externo.</p>
</div>
</section>

<section id="ota">
<div class="panel">
<h3>Atualização OTA</h3>
<form method="POST" action="/api/ota" enctype="multipart/form-data">
<input type="file" name="firmware" accept=".bin" required>
<button class="action" type="submit">Atualizar firmware</button>
</form>
<small class="note">Envie o arquivo firmware.bin gerado pelo PlatformIO.</small>
</div>
</section>
</main>
</div>
<script>
const q=id=>document.getElementById(id);
async function json(url,opt){const r=await fetch(url,opt);const d=await r.json();if(!r.ok)throw new Error(d.error||'erro');return d}
document.querySelectorAll('#nav button').forEach(b=>b.onclick=()=>{document.querySelectorAll('section').forEach(s=>s.classList.remove('active'));document.querySelectorAll('#nav button').forEach(x=>x.classList.remove('active'));q(b.dataset.tab).classList.add('active');b.classList.add('active');q('title').textContent=b.textContent;if(b.dataset.tab==='performance')loadPerformance();if(b.dataset.tab==='history')loadHistory();if(b.dataset.tab==='logs')loadLogs();if(b.dataset.tab==='config')loadConfig()});
async function refresh(){try{const [s,m]=await Promise.all([json('/api/status'),json('/api/sensors/latest')]);q('connection').textContent='ESP32 online';q('connection').className='badge status-ok';q('wifi').textContent=s.wifi_connected?'conectado':'offline';q('rssi').textContent=s.rssi_dbm;q('ip').textContent=s.station_ip||s.ap_ip||'--';q('mqtt').textContent=s.mqtt_connected?'conectado':'offline';q('temperature').textContent=m.dht_valid?Number(m.temperature).toFixed(1):'N/A';q('humidity').textContent=m.dht_valid?Number(m.humidity).toFixed(1):'N/A';q('luminosity').textContent=m.luminosity;q('air').textContent=m.air_quality}catch(e){q('connection').textContent='ESP32 offline';q('connection').className='badge status-bad'}}
async function actuator(a,c){try{await json('/api/actuator',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams({actuator:a,command:c})});alert('Comando executado no ESP32')}catch(e){alert(e.message)}}
async function loadPerformance(){const p=await json('/api/performance');q('cpu').textContent=Number(p.cpu_usage_percent_estimated||0).toFixed(1);q('heap').textContent=p.heap_free;q('flash').textContent=p.flash_bytes;q('tasksCount').textContent=p.task_count;q('memory').textContent=JSON.stringify({heap_total:p.heap_total,heap_free:p.heap_free,heap_min_free:p.heap_min_free,psram_available:p.psram_available,psram_total:p.psram_total,psram_free:p.psram_free,cpu_frequency_mhz:p.cpu_frequency_mhz,wifi_rssi_dbm:p.wifi_rssi_dbm},null,2);q('timings').innerHTML=(p.function_timings||[]).map(x=>'<tr><td>'+x.name+'</td><td>'+x.last_us+'</td><td>'+x.avg_us+'</td><td>'+x.max_us+'</td></tr>').join('');q('tasks').innerHTML=(p.tasks||[]).map(x=>'<tr><td>'+x.name+'</td><td>'+x.stack_high_water_bytes+' bytes</td></tr>').join('')}
async function loadHistory(){const data=await json('/api/sensors/history');const c=q('chart'),ctx=c.getContext('2d');ctx.clearRect(0,0,c.width,c.height);if(!data.length){ctx.fillText('Sem histórico disponível',20,30);return}const vals=data.filter(x=>x.dht_valid).map(x=>Number(x.temperature));if(!vals.length)return;const min=Math.min(...vals)-1,max=Math.max(...vals)+1;ctx.beginPath();ctx.strokeStyle='#2f6f4f';ctx.lineWidth=2;vals.forEach((v,i)=>{const x=i*(c.width/(Math.max(vals.length-1,1)));const y=c.height-((v-min)/(max-min))*c.height;if(i===0)ctx.moveTo(x,y);else ctx.lineTo(x,y)});ctx.stroke()}
async function loadConfig(){const c=await json('/api/config');q('sampleMs').value=c.sensor_interval_ms;q('mqttHost').value=c.mqtt_host;q('ssid').value=c.wifi_ssid}
async function saveSample(){await json('/api/config/sample',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams({interval_ms:q('sampleMs').value})});alert('Intervalo salvo')}
async function saveMqtt(){await json('/api/config/mqtt',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams({host:q('mqttHost').value})});alert('MQTT atualizado')}
async function saveWifi(){await json('/api/config/wifi',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams({ssid:q('ssid').value,password:q('password').value})});alert('Wi-Fi salvo. O ESP tentará a nova rede.')}
async function sleepNow(){await json('/api/energy/light-sleep',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams({seconds:q('sleepSeconds').value})});alert('Light Sleep agendado')}
async function loadLogs(){const r=await fetch('/api/logs');q('logText').textContent=await r.text()}
refresh();setInterval(refresh,5000);
</script>
</body>
</html>
)HTML";
