import { createElement, render } from "./engine.js";

const element = createElement(
  "div",
  { id: "app" },
  createElement("h1", null, "Hello Fiber"),
  createElement("p", null, "world"),
);

const container = document.getElementById("root");

render(
  createElement(
    "div",
    null,
    createElement("p", null, "A"),
    createElement("p", null, "B"),
  ),
  container,
);
