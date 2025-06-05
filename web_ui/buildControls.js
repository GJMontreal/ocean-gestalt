// buildControls.js
export function buildControls(tree, path = []) {
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
        control.id = fullPath.join("_");
        controlRow.appendChild(labelEl);
        controlRow.appendChild(control);
        elements.push(controlRow);
      }
    } else if (isGroup(node)) {
      const children = buildControls(node, fullPath);
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

function createControl(label, node, path) {
  const wrapper = document.createElement("div");

  switch (node.type) {
    case "bool":
      const toggle = document.createElement("input");
      toggle.type = "checkbox";
      toggle.checked = node.value;
      wrapper.appendChild(toggle);
      break;

    case "float":
      const slider = document.createElement("input");
      slider.type = "range";
      slider.min = node.min ?? 0;
      slider.max = node.max ?? 1;
      slider.step = 0.01;
      slider.value = node.value;
      wrapper.appendChild(slider);
      break;

    case "vec4":
      const color = document.createElement("input");
      color.type = "color";
      color.value = rgbToHex(node.value);
      wrapper.appendChild(color);
      break;

    default:
      const fallback = document.createElement("code");
      fallback.textContent = `[unhandled type: ${node.type}]`;
      wrapper.appendChild(fallback);
  }

  return wrapper;
}

function rgbToHex(vec4) {
  const [r, g, b] = vec4.map(x => Math.round(x * 255));
  return `#${[r, g, b].map(n => n.toString(16).padStart(2, "0")).join("")}`;
}
