import { createElement, render } from "./engine.js";

const element = createElement(
  "div",
  { id: "app" },
  createElement("h1", null, "Hello Fiber"),
  createElement("p", null, "This is incremental rendering"),
);

const container = document.getElementById("root");

render(element, container);
