#################################
Intercepting Filter
#################################



Problem 问题
===================================
You want to intercept and manipulate a request and a response before and after the request is processed.


要在请求被处理之前，之后拦截并操作一个请求和它的响应

* 预处理
* 后处理


Forces 约束
===================================

* You want centralized, common processing across requests, such as checking the data-encoding scheme of each request, logging information about each request, or compressing an outgoing response.
* You want pre and postprocessing components loosely coupled with core request-handling services to facilitate unobtrusive addition and removal.
* You want pre and postprocessing components independent of each other and self contained to facilitate reuse.

Solution
===================================

Use an Intercepting Filter as a pluggable filter to pre and postprocess requests and responses. A filter manager combines loosely coupled filters in a chain, delegating control to the appropriate filter. In this way, you can add, remove, and combine these filters in various ways without changing existing code.


Strategies
===================================
Standard Filter Strategy
Custom Filter Strategy
Base Filter Strategy
Template Filter Strategy
Web Service Message Handling Strategies
Custom SOAP Filter Strategy
JAX RPC Filter Strategy


Consequences
===================================

Centralizes control with loosely coupled handlers
Improves reusability
Declarative and flexible configuration
Information sharing is inefficient

Related Patterns
===================================

* Front Controller

The controller solves some similar problems, but is better suited to handling core processing.

* Decorator [GoF]

The Intercepting Filter is related to the Decorator, which provides for dynamically pluggable wrappers.

* Template Method [GoF]

The Template Method is used to implement the Template Filter strategy.

* Interceptor [POSA2]

The Intercepting Filter is related to the Interceptor, which allows services to be added transparently and triggered automatically.

* Pipes and Filters [POSA1]

The Intercepting Filter is related to Pipes and Filters.