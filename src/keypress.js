
/* Emscripten-Generated Code */
function simulateKeyEvent(eventType, keyCode, code, key, target) {
  var props = { keyCode, charCode: keyCode, view: window, bubbles: true, cancelable: true };
  if (code) props['code'] = code;
  if (key) props['key'] = key;
  var event = new KeyboardEvent(eventType, props);
  if (!target) target = document;
  return target.dispatchEvent(event);
}

function simulateKeyDown(keyCode, code = undefined, key = undefined, target = undefined) {
  var doDefault = simulateKeyEvent('keydown', keyCode, code, key, target);
  // As long as not handler called `preventDefault` we also send a keypress
  // event.
  if (doDefault) {
    simulateKeyEvent('keypress', keyCode, code, key, target);
  }
}

function simulateKeyUp(keyCode, code = undefined, target = undefined) {
  simulateKeyEvent('keyup', keyCode, code, target);
}

function simulateKeyDownUp(keyCode, code = undefined, target = undefined) {
  simulateKeyDown(keyCode, code, target);
  setTimeout(()=> { simulateKeyUp(keyCode, code, target);}, 100);
  
}
