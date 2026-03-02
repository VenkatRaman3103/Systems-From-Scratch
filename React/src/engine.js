export function createTextElement(text) {
  return {
    type: "TEXT_ELEMENT",
    props: {
      nodeValue: text,
      children: [],
    },
  };
}
//
export function createElement(type, props, ...children) {
  return {
    type: type,
    props: {
      ...props,
      children: children
        .flat()
        .map((child) =>
          typeof child == "object" ? child : createTextElement(child),
        ),
    },
  };
}
