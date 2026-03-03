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
    fiber.type == "TEXT_ELEMENT"
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
      dom[name] = nextProps[name];
    });
}

// FIBER
let nextUnitOfWork = null;
let wipRoot = null;

export function render(element, container) {
  wipRoot = {
    dom: container,
    props: {
      children: [element],
    },
    parent: null,
    child: null,
    sibling: null,
  };

  nextUnitOfWork = wipRoot;
}

// SCHEDULER
export function workLoop(deadline) {
  let shouldYield = false;

  console.log(deadline);

  while (nextUnitOfWork && !shouldYield) {
    nextUnitOfWork = performUnitOfWork(nextUnitOfWork);
    shouldYield = deadline.timeRemaining() < 1;
  }

  if (!nextUnitOfWork && wipRoot) {
    commitRoot();
  }

  requestIdleCallback(workLoop);
}

requestIdleCallback(workLoop);

// PERFORM UNIT OF WORK
export function performUnitOfWork(fiber) {
  // container
  if (!fiber.dom) {
    fiber.dom = createDomNode(fiber);
  }

  let elements = fiber.props.children || [];
  let prevSibling = null;
  let index = 0;

  while (index < elements.length) {
    let element = elements[index];

    let newFiber = {
      type: element.type,
      props: element.props,
      parent: fiber,
      dom: null,
      child: null,
      sibling: null,
    };

    if (index == 0) {
      fiber.child = newFiber;
    } else {
      prevSibling.sibling = newFiber;
    }

    prevSibling = newFiber;

    index++;
  }

  if (fiber.child) {
    return fiber.child;
  }

  let nextFiber = fiber;

  while (nextFiber) {
    if (nextFiber.sibling) {
      return nextFiber.sibling;
    }

    nextFiber = nextFiber.parent;
  }

  return null;
}

// COMMIT
export function commitRoot() {
  commitWork(wipRoot.child);
  wipRoot = null;
}

export function commitWork(fiber) {
  if (!fiber) return;

  let domParentFiber = fiber.parent;
  while (!domParentFiber.dom) {
    domParentFiber = domParentFiber.parent;
  }

  const domParent = domParentFiber.dom;

  if (fiber.dom) {
    domParent.appendChild(fiber.dom);
  }

  commitWork(fiber.child);
  commitWork(fiber.sibling);
}
