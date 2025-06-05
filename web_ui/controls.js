export function buildControls(obj, path = '', parent = document.getElementById('controls')) {
  if (!obj || typeof obj !== 'object') return;

  const entries = Object.entries(obj).filter(([k]) => k !== '_meta');
  const meta = obj._meta || {};
  const groupLabel = meta.display || path.split('/').pop();

  const group = document.createElement('fieldset');
  const legend = document.createElement('legend');
  legend.textContent = groupLabel;
  group.appendChild(legend);
  parent.appendChild(group);

  for (const [key, value] of entries) {
    const fullPath = path ? `${path}/${key}` : key;
    if (typeof value !== 'object') continue;

    const vMeta = value._meta || {};
    const label = vMeta.display || key;

    if (value.type === 'bool') {
      const wrapper = document.createElement('div');
      const labelElem = document.createElement('label');
      labelElem.textContent = label;

      const checkbox = document.createElement('input');
      checkbox.type = 'checkbox';
      checkbox.checked = value.value;
      checkbox.onchange = () => sendUpdate(fullPath, checkbox.checked);

      labelElem.appendChild(checkbox);
      wrapper.appendChild(labelElem);
      group.appendChild(wrapper);

    } else if (value.type === 'float') {
      const wrapper = document.createElement('div');
      const labelElem = document.createElement('label');
      labelElem.textContent = label;

      const slider = document.createElement('input');
      slider.type = 'range';
      slider.min = value.min ?? 0;
      slider.max = value.max ?? 1;
      slider.step = 0.01;
      slider.value = value.value;

      slider.oninput = () => sendUpdate(fullPath, parseFloat(slider.value));

      labelElem.appendChild(slider);
      wrapper.appendChild(labelElem);
      group.appendChild(wrapper);

    } else if (value.type === 'vec3' || value.type === 'vec4') {
      const isColor = vMeta.control === 'color' && value.value.length >= 3;
      const wrapper = document.createElement('div');
      wrapper.style.marginBottom = '0.5em';

      const labelElem = document.createElement('label');
      labelElem.textContent = label + ': ';
      wrapper.appendChild(labelElem);

      if (isColor) {
        const input = document.createElement('input');
        input.type = 'color';

        const [r, g, b] = value.value.map(x => Math.round((x ?? 0) * 255));
        input.value = `#${[r, g, b].map(x => x.toString(16).padStart(2, '0')).join('')}`;

        input.oninput = () => {
          const hex = input.value;
          const r = parseInt(hex.slice(1, 3), 16) / 255;
          const g = parseInt(hex.slice(3, 5), 16) / 255;
          const b = parseInt(hex.slice(5, 7), 16) / 255;
          const out = [r, g, b];
          if (value.value.length === 4) out.push(value.value[3]);
          sendUpdate(fullPath, out);
        };

        wrapper.appendChild(input);
      } else {
        const range = vMeta.range || [0, 1];
        const names = ['X', 'Y', 'Z', 'W'];

        value.value.forEach((v, i) => {
          const slider = document.createElement('input');
          slider.type = 'range';
          slider.min = range[0];
          slider.max = range[1];
          slider.step = 0.01;
          slider.value = v;

          const sublabel = document.createElement('label');
          sublabel.textContent = `${names[i] ?? `v${i}`}: `;
          sublabel.style.marginRight = '0.5em';

          slider.oninput = () => {
            const newVec = [...value.value];
            newVec[i] = parseFloat(slider.value);
            sendUpdate(fullPath, newVec);
          };

          sublabel.appendChild(slider);
          wrapper.appendChild(sublabel);
        });
      }

      group.appendChild(wrapper);
    } else {
      buildControls(value, fullPath, group);
    }
  }
}

function sendUpdate(path, value) {
  fetch('/api/update', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ path, value })
  }).catch(err => console.error('Failed to send update', err));
}
