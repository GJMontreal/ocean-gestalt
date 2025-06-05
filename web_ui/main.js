import { buildControls } from './buildControls.js';

fetch('/api/params')
  .then(res => res.json())
  .then(data => {
    const container = document.getElementById("controls");
    const controls = buildControls(data);
    controls.filter(Boolean).forEach(
      el => container.appendChild(el)
    );
    // controls.forEach(el => container.appendChild(el));
  });
