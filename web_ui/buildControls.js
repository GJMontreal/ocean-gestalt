// buildControls.js



export function buildControls(tree, path = [], apiBase) {
  const elements = [];

  if (!Array.isArray(path)) {
    path = [path]; // fix non-array path inputs
  }
  
  for (const key in tree) {
    if (key === "_meta") continue;

    const node = tree[key];
    const fullPath = [...path, key];
    const label = node?._meta?.display || key;

    if (isControl(node)) {
      const controlRow = document.createElement("div");
      controlRow.className = "control-row";

      const labelEl = document.createElement("label");
      labelEl.textContent = label;
      labelEl.setAttribute("for", fullPath.join("_"));

      const valueElem = document.createElement("span");
      valueElem.className = "control-value";

      const control = createControl(label, node, fullPath, valueElem);
      if (control) {

        controlRow.appendChild(labelEl);

        controlRow.appendChild(control.wrapper);
 
        control.input.oninput = () => {
          const newValue = control.getValue();
          // valueElem.textContent = newValue;
          postUpdate(fullPath, newValue, apiBase, control); 
        };

        fetch(`${apiBase}/api/${fullPath.join('/')}`)
          .then(res => res.json())
          .then(data => {
            control.setValue(data.value);
          })
          .catch(err => console.error("Failed to load value for", fullPath.join('/'), err));

        controlRow.appendChild(valueElem);

        elements.push(controlRow);
      }
    } else if (isGroup(node)) {
      const children = buildControls(node, fullPath, apiBase);
      if (children.length > 0) {
        const group = document.createElement("fieldset");
        const legend = document.createElement("legend");
        legend.textContent = label;
        group.appendChild(legend);
        children.forEach(child => group.appendChild(child));
        elements.push(group);
      }
    } else {
      console.warn("Unrecognized node structure at", fullPath.join("."), node);
    }
  }

  return elements;
}

function isControl(node) {
  return (
    typeof node === "object" &&
    node !== null &&
    !Array.isArray(node) &&
    "type" in node 
  );
}

function isGroup(node) {
  return (
    typeof node === "object" &&
    node !== null &&
    !Array.isArray(node) &&
    !isControl(node)
  );
}

function createNamedDiv(name) {
  const spacer = document.createElement("div");
  spacer.className = name;
  return spacer;
}

// right now we're driving
// 
function createControl(label, node, path, valueElem) {
  const wrapper = document.createElement("div");
  wrapper.id = path.join("_");

  let input = null;
  let getValue = () => "";
  let setValue = (value) => ""; 

  if (node._meta?.control === "directionKnob") {
    const wrapper = document.createElement("div");
    wrapper.style.width = "150px";
    wrapper.style.height = "150px";

    const input = document.createElement("div");
    wrapper.appendChild(input);

    const knob = new VanillaKnob(input, {
      angleArc: 360,
      angleOffset: 0,
      min: 0,
      max: 360,
      value: 0,
      thickness: 0.3,
      displayInput: true,
    });

    const getValue = () => knob.val();
    const setValue = (v) => knob.val(Number(v));

    return { wrapper, input: knob, getValue, setValue };
  }

  switch (node.type) {
    case "bool":
      input = document.createElement("input");
      input.type = "checkbox";
      wrapper.appendChild(input);

      getValue = () => input.checked;
      setValue = (value) => {
        input.checked = value;
        valueElem.textContent = value;
      }
      break;

    case "float":
      input = document.createElement("input");
      input.type = "range";
      input.min = node.min ?? 0;
      input.max = node.max ?? 1;
      input.step = 0.001;
      input.style.flex = "1";

      wrapper.style.display = "flex";
      wrapper.style.alignItems = "center";
      wrapper.style.gap = "8px";

      const numberInput = document.createElement("input");
      numberInput.type = "number";
      numberInput.min = node.min ?? 0;
      numberInput.max = node.max ?? 1;
      numberInput.step = 0.001;
      numberInput.style.width = "5em";
      numberInput.style.fontFamily = "monospace";
      numberInput.style.flexShrink = "0";

      input.addEventListener("input", () => { numberInput.value = Number(input.value).toFixed(3); });
      numberInput.addEventListener("input", () => {
        input.value = numberInput.value;
        input.dispatchEvent(new Event("input"));
      });
      numberInput.addEventListener("keydown", (e) => {
        if (e.key === "Enter") {
          input.value = numberInput.value;
          input.dispatchEvent(new Event("input"));
        }
      });

      wrapper.appendChild(input);
      wrapper.appendChild(numberInput);
      getValue = () => Number(input.value);
      setValue = (value) => {
        input.value = value;
        numberInput.value = value.toFixed(3);
      }
      break;

    case "vec4":
      input = document.createElement("input");
      input.type = "color";
      wrapper.appendChild(input);
      getValue = () => hexToVec4(input.value);
      setValue = (value) => {
        input.value = rgbToHex(value);
        valueElem.textContent = `[${value.map(v => v.toFixed(2)).join(",")}]`;
      }
      break;

    default:
      const fallback = document.createElement("code");
      fallback.textContent = `[unhandled type: ${node.type}]`;
      wrapper.appendChild(fallback);
  }

  return {wrapper, input, getValue, setValue };
}

function rgbToHex(vec4) {
  const [r, g, b] = vec4.map(x => Math.round(x * 255));
  return `#${[r, g, b].map(n => n.toString(16).padStart(2, "0")).join("")}`;
}

function hexToVec4(hex) {
  // Strip leading '#' if present
  hex = hex.replace(/^#/, "");

  // Parse r, g, b components (2 hex chars each)
  const r = parseFloat((parseInt(hex.slice(0, 2), 16) / 255).toFixed(2));
  const g = parseFloat((parseInt(hex.slice(2, 4), 16) / 255).toFixed(2));
  const b = parseFloat((parseInt(hex.slice(4, 6), 16) / 255).toFixed(2));

  return [r, g, b, 1.0];
}

function postUpdate(path, value, apiBase, control) {
  const endpoint = path.join("/");
  const duration = parseFloat(document.getElementById('transition-duration')?.value ?? 0);
  const body = { path: path.join("."), value };
  if (duration > 0) body.duration = duration;

  fetch(`${apiBase}/api/${endpoint}`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body)
  })
    .then(res =>{
      if (!res.ok) throw new Error(`Server responded with status ${res.status}`);
      return res.json();
    })
    .then(data =>{
        if (data.value !== undefined) {
          control.setValue(data.value);
        }
    })
  .catch(err => console.error("POST failed:", err));
}
