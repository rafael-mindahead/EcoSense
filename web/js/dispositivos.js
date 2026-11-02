import { getDevices } from "./api.js";
const root=document.querySelector("#devices"), msg=document.querySelector("#message");
const esc=v=>String(v??"—").replace(/[&<>"']/g,c=>({"&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;","'":"&#39;"}[c]));
async function load(){try{const r=await getDevices(), rows=r.data||[]; root.innerHTML=rows.length?`<table><thead><tr><th>Dispositivo</th><th>Status</th><th>Última comunicação</th><th>Detalhes</th></tr></thead><tbody>${rows.map(d=>`<tr><td>${esc(d.name||d.device_name||d.id)}</td><td>${esc(d.status||"Não informado")}</td><td>${esc(d.last_seen||d.last_communication||"—")}</td><td>${esc(d.description||d.location||"—")}</td></tr>`).join("")}</tbody></table>`:"Nenhum dispositivo cadastrado.";}catch(e){msg.textContent="Não foi possível carregar os dispositivos. Verifique a conexão com a API.";root.textContent="";}}
load();
