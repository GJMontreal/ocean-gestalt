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

      
      const control = createControl(label, node, fullPath);
      if (control) {

        controlRow.appendChild(labelEl);

        controlRow.appendChild(control.wrapper);

        const valueElem = document.createElement("span");
        valueElem.className = "control-value";
        valueElem.textContent = control.getValue();
        control.input.oninput = () => {
          const newValue = control.getValue();
          valueElem.textContent = newValue;
          postUpdate(fullPath, newValue, apiBase);
        };
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
    "type" in node &&
    "value" in node
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

function createControl(label, node, path) {
  const wrapper = document.createElement("div");
  wrapper.id = path.join("_");

  let input = null;
  let getValue = () => "";

  switch (node.type) {
    case "bool":
      input = document.createElement("input");
      input.type = "checkbox";
      input.checked = node.value;
      wrapper.appendChild(input);
      getValue = () => input.checked ? "on" : "off";
      break;

    case "float":
      input = document.createElement("input");
      input.type = "range";
      input.min = node.min ?? 0;
      input.max = node.max ?? 1;
      input.step = 0.01;
      input.value = node.value;
      wrapper.appendChild(input);
      getValue = () => Number(input.value).toFixed(2);
      break;

    case "vec4":
      input = document.createElement("input");
      input.type = "color";
      input.value = rgbToHex(node.value);
      wrapper.appendChild(input);
      getValue = () => JSON.stringify(hexToVec4(input.value));
      break;

    default:
      const fallback = document.createElement("code");
      fallback.textContent = `[unhandled type: ${node.type}]`;
      wrapper.appendChild(fallback);
  }

  return {wrapper,input ,getValue };
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

function postUpdate(path, value, apiBase) {
  fetch(`${apiBase}/api/update`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      path: path.join("."),  // flatten path array
      value
    })
  }).catch(err => console.error("POST failed:", err));
}
