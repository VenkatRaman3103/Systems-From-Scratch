import { createElement, render } from "./engine.js";

const container = document.getElementById("root");

function App() {
  return createElement(
    "div",
    null,
    createElement("h1", null, "Hello"),
    createElement("p", null, "World"),
  );
}

render(createElement(App, null), container);
