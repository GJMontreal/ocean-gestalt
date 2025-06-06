import { buildControls } from './buildControls.js';

const { host, port } = window.apiConfig;
const apiBase = `http://${host}:${port}`;

fetch(`${apiBase}/api/params`)
  .then(res => res.json())
  .then(data => {
    const container = document.getElementById("controls");
    const controls = buildControls(data, [], apiBase);
    controls.filter(Boolean).forEach(
      el => container.appendChild(el)
    );
  });
