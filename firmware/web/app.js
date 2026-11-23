// DaemonHID Web Dashboard Controller
const API = {
  status: '/api/status',
  payloads: '/api/payloads',
  execute: '/api/execute',
  abort: '/api/abort'
};

const dom = {
  connIndicator: document.getElementById('connIndicator'),
  sdBadge: document.getElementById('sdBadge'),
  firmwareVersion: document.getElementById('firmwareVersion'),
  alertBanner: document.getElementById('alertBanner'),
  alertTitle: document.getElementById('alertTitle'),
  countdownTimer: document.getElementById('countdownTimer'),
  btnAbortBanner: document.getElementById('btnAbortBanner'),
  statHid: document.getElementById('statHid'),
  statRam: document.getElementById('statRam'),
  statUptime: document.getElementById('statUptime'),
  statIp: document.getElementById('statIp'),
  payloadList: document.getElementById('payloadList'),
  btnRefreshPayloads: document.getElementById('btnRefreshPayloads'),
  btnDemoText: document.getElementById('btnDemoText'),
  btnDemoPing: document.getElementById('btnDemoPing'),
  btnAbortGlobal: document.getElementById('btnAbortGlobal'),
  toast: document.getElementById('toast')
};

let pollInterval = null;

function showToast(message, duration = 3000) {
  dom.toast.textContent = message;
  dom.toast.classList.remove('hidden');
  setTimeout(() => {
    dom.toast.classList.add('hidden');
  }, duration);
}

function formatUptime(seconds) {
  const h = Math.floor(seconds / 3600).toString().padStart(2, '0');
  const m = Math.floor((seconds % 3600) / 60).toString().padStart(2, '0');
  const s = Math.floor(seconds % 60).toString().padStart(2, '0');
  return `${h}:${m}:${s}`;
}

async function updateStatus() {
  try {
    const res = await fetch(API.status);
    if (!res.ok) throw new Error('HTTP ' + res.status);
    const data = await res.json();

    dom.connIndicator.classList.add('live');
    dom.firmwareVersion.textContent = 'v' + (data.version || '0.1.0');

    // MicroSD Status
    if (data.sd_ready) {
      dom.sdBadge.textContent = 'SD ACTIVA';
      dom.sdBadge.className = 'badge badge-success';
    } else {
      dom.sdBadge.textContent = 'SIN SD';
      dom.sdBadge.className = 'badge badge-danger';
    }

    // Telemetry
    dom.statHid.textContent = data.hid_state || 'inactivo';
    dom.statRam.textContent = (data.free_ram > 0) ? `${(data.free_ram / 1024).toFixed(1)} KB` : 'N/D';
    dom.statUptime.textContent = formatUptime(data.uptime_s || 0);
    dom.statIp.textContent = data.ip || '192.168.4.1';

    // Banner de Alerta / Cuenta Regresiva
    if (data.hid_state === 'cuenta_regresiva') {
      dom.alertBanner.classList.remove('hidden');
      dom.alertTitle.textContent = 'DEMOSTRACIÓN ARMADA';
      dom.countdownTimer.textContent = data.countdown;
    } else if (data.hid_state === 'escribiendo') {
      dom.alertBanner.classList.remove('hidden');
      dom.alertTitle.textContent = 'ESCRIBIENDO TEXTO HID...';
      dom.countdownTimer.textContent = '0';
    } else {
      dom.alertBanner.classList.add('hidden');
    }
  } catch (err) {
    dom.connIndicator.classList.remove('live');
    dom.statHid.textContent = 'Desconectado';
  }
}

async function loadPayloads() {
  try {
    const res = await fetch(API.payloads);
    if (!res.ok) throw new Error('HTTP ' + res.status);
    const data = await res.json();

    dom.payloadList.innerHTML = '';
    const files = data.payloads || [];

    if (files.length === 0) {
      dom.payloadList.innerHTML = '<div class="empty-state">No se encontraron scripts en <code>/payloads/</code> de la MicroSD.</div>';
      return;
    }

    files.forEach(file => {
      const item = document.createElement('div');
      item.className = 'payload-item';
      item.innerHTML = `
        <span class="payload-name">📄 ${file}</span>
        <button class="btn btn-primary btn-sm" onclick="triggerPayload('${file}')">Lanzar (5s)</button>
      `;
      dom.payloadList.appendChild(item);
    });
  } catch (err) {
    dom.payloadList.innerHTML = '<div class="empty-state">Error al consultar los scripts de la MicroSD.</div>';
  }
}

async function triggerPayload(filename) {
  try {
    const res = await fetch(`${API.execute}?file=${encodeURIComponent(filename)}`, { method: 'POST' });
    const data = await res.json();
    if (data.success) {
      showToast(`⚡ ${data.message}`);
    } else {
      showToast(`⚠️ Error: ${data.message}`);
    }
    updateStatus();
  } catch (err) {
    showToast('⚠️ Error de comunicación al disparar payload.');
  }
}
window.triggerPayload = triggerPayload;

async function abortExecution() {
  try {
    const res = await fetch(API.abort, { method: 'POST' });
    const data = await res.json();
    showToast(`🛑 ${data.message}`);
    updateStatus();
  } catch (err) {
    showToast('⚠️ Error al enviar comando de aborto.');
  }
}

// Event Listeners
dom.btnRefreshPayloads.addEventListener('click', () => {
  loadPayloads();
  showToast('↻ Lista de scripts actualizada');
});

dom.btnAbortBanner.addEventListener('click', abortExecution);
dom.btnAbortGlobal.addEventListener('click', abortExecution);

dom.btnDemoText.addEventListener('click', () => triggerPayload('DEMO_TEXT'));
dom.btnDemoPing.addEventListener('click', () => triggerPayload('DEMO_PING'));

// Init
updateStatus();
loadPayloads();
pollInterval = setInterval(updateStatus, 1500);
