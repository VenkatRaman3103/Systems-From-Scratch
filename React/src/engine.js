// VIRTUAL DOM
export function createTextElement(text) {
  return {
    type: "TEXT_ELEMENT",
    props: {
      nodeValue: text,
      children: [],
    },
  };
}

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

// REAL DOM
export function createDomNode(fiber) {
  if (fiber.type == "TEXT_ELEMENT") {
    return document.createTextNode(fiber.props.nodeValue);
  }

  let dom =
    vnode.type == "TEXT_ELEMENT"
      ? document.createTextNode(fiber.props.nodeValue)
      : document.createElement(fiber.type);

  updatedDom(dom, {}, fiber.props);

  return dom;
}

export function updatedDom(dom, prevProps, nextProps) {
  // remove old props
  Object.keys(prevProps || {})
    .filter((key) => key != "children")
    .forEach((name) => {
      console.log(name);
      dom[name] = "";
    });

  // add new props
  Object.keys(nextProps || {})
    .filter((key) => key != "children")
    .forEach((name) => {
      console.log(name);
      dom[name] = nextProps.props[name];
    });
}

export function render(vnode, container) {
  let dom = createDomNode(vnode);

  vnode.props.children.forEach((child) => {
    render(child, dom);
  });

  container.appendChild(dom);
}
