#pragma once
#define HTML_BUNDLE_SIZE 58201
static std::string html_bundle = 
R"773ff122(<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1" />
    <title>SDUI</title>

    <style>
      :root {
        --pageBg: #202020;
        --secondaryBg: #303030;
        --fg: #fff;
        --borders: #303030;
      }

      body {
        font-family: Arial, sans-serif;
        margin: 0;
        padding: 0;
        background-color: var(--pageBg);
        color: #fff;
      }

      .hello-message {
        text-align: center;
        margin-top: 2%;

        > code {
          font-size: 1rem;
          background-color: #101010;
          border-radius: 12px;
          padding: 12px 20px;
        }
      }

      .header {
        background-color: var(--secondaryBg);
        padding: 16px 20px;
        display: flex;
        justify-content: space-between;
        align-items: center;

        .title {
          font-size: 22px;
          font-weight: bold;
        }
      }

      select {
        padding: 8px 16px;
        margin-right: 8px;
        background-color: #303030;
        color: #fff;
        border: none;
        cursor: pointer;
        border-radius: 4px;
        font-weight: bold;
      }

      option {
        background-color: #303030;
        color: #fff;
        border: none;
        cursor: pointer;
        border-radius: 4px;
        font-weight: bold;
        font-size: 16px;
      }

      input[type="text"],
      input[type="number"] {
        padding: 8px 16px;
        margin-right: 8px;
        background-color: #303030;
        color: #fff;
        border: none;
        border-radius: 4px;
        font-weight: bold;
      }

      /* input[type="range"] {
        width: 100%;
        -webkit-appearance: none;
        background: #303030;
        accent-color: #999;
        border-radius: 4px;
        height: 8px;
        cursor: pointer; 
      } */

      input[type="range"],
      input[type="checkbox"] {
        accent-color: #999;
        cursor: pointer;
      }

      a {
        color: #1895d3;
        text-decoration: none;
      }

      textarea {
        padding: 8px 16px;
        margin-right: 8px;
        background-color: #303030;
        color: #fff;
        border: none;
        border-radius: 4px;
        font-weight: bold;
        width: 100%;
        height: 50px;
      }

      button {
        padding: 8px 16px;
        /* margin-right: 8px; */
        background-color: #303030;
        color: #fff;
        border: none;
        cursor: pointer;
        border-radius: 4px;
        font-weight: bold;
      }

      button:hover {
        filter: brightness(0.9);
      }

      button:active {
        filter: brightness(0.8);
      }

      .buttons {
        /* margin-bottom: 16px; */
        font-size: 20px;
        font-weight: bold;

        &.gap {
          display: flex;
          justify-content: center;
          gap: 6px;
        }
      }

      .btn-primary {
        background-color: #303030;
        color: #fff;
      }

      .btn-danger {
        background-color: #ff0000;
        color: #fff;
      }

      .btn-blue {
        background-color: #1895d3;
        color: #fff;
      }

      .btn-warn {
        background-color: #df4309;
        color: #fff;
      }

      @keyframes rainbow {
        /* to {background-position:0% 100%} */
        /* draw manually, note we have to handle full image */
        0% {
          background-position: 0% 25%;
        }

        25% {
          background-position: 100% 50%;
        }

        50% {
          background-position: 0% 75%;
        }

        75% {
          background-position: 100% 100%;
        }

        100% {
          background-position: 0% 25%;
        }
      }

      .content {
        padding: 14px 24px;
        box-sizing: border-box;
        /* padding-top: ; */

        .bar {
          display: flex;
          justify-content: space-between;
        }

        /* turn bar into column if width is too small */
        @media (max-width: 800px) {
          .bar {
            gap: 16px;
            flex-direction: column;
            align-items: center;
          }

          #btn-output-folder {
            display: none;
          }
        }

        hr {
          border: 1px solid #303030;
          margin: 16px 0;
        }

        .title {
          font-size: 22px;
          font-weight: bold;
        }

        .subtitle {
          font-size: 18px;
          font-weight: bold;
          margin-bottom: 8px;
        }

        .framed {
          border: 1px solid #303030;
          padding: 16px;
          border-radius: 4px;
          margin-bottom: 16px;
          width: fit-content;
          display: flex;
          flex-direction: column;
          align-items: center;
          box-sizing: border-box;
          transition: all 0.5s;
        }

        .io {
          display: flex;
          justify-content: center;
          gap: 16px;
        }

        /* loading */
        .rainbow {
          display: flex;
          justify-content: center;
          align-items: center;
          width: 512px;
          height: 512px;
          max-width: 100%;
          border-radius: 12px;
          animation: rainbow 12s ease infinite;
          /* make a new animated gradient with percentages for animation. Note we need addional values at end. */
          /* background: linear-gradient(300deg, violet, darkviolet, blue, navy); */
          /* background-size: 180% 180%; */
          background: linear-gradient(
              206deg,
              #9dc729,
              #136821,
              #1895d3,
              #b1404a,
              violet,
              darkviolet,
              blue,
              navy
            )
            0% 0% / 400% 400%;
        }
      }

      @media (max-width: 800px) {
        .content {
          width: 100%;
          margin: 0;
          padding: 12px 2px;

          .io {
            flex-direction: column;
            gap: 4px;
          }

          .framed {
            margin: 8px 0;
            width: 100%;
          }
        }

        .gen-options {
          min-width: 100%;
        }
      }

      .gen-options {
        width: 100%;
        min-width: 25vw;
        display: flex;
        gap: 8px;
        align-items: center;
        flex-direction: column;

        #image-preview {
          border: 1px solid #303030;
          border-radius: 4px;
          max-width: 100%;
          max-height: 256px;
        }

        > div {
          width: calc(100% - 16px);

          > textarea {
            width: calc(100% - 32px);
          }
        }

        .slider {
          display: flex;
          gap: 8px;
          align-items: center;

          > input {
            width: calc(100% - 50px);
          }
        }

        .grid {
          display: grid;
          grid-template-columns: 1fr 1fr;
          gap: 8px;
        }

        .grid-20-80 {
          grid-template-columns: 20% 80%;
        }

        .group {
          display: flex;
          gap: 16px;
          justify-content: center;

          > div {
            width: 49%;
          }
        }

        .opt-save {
          font-weight: bold;
          text-align: center;
          align-self: center;
        }

        @media (max-width: 800px) {
          .group {
            flex-direction: column;

            > div {
              width: 100%;
            }
          }

          .grid {
            grid-template-columns: 1fr !important;
          }

          .opt-save {
            order: 1;

            > label {
              display: flex;
            }
          }
        }
      }

      .gen-options img {
      }

      .label {
        font-weight: bold;
        margin: 4px;
      }

      @starting-style {
        #more-options {
          opacity: 0;
          height: 0;
        }

        .modal {
          opacity: 0;
        }
      }

      #more-options {
        height: fit-content;
        transition: opacity 0.25s, transform 0.25s, height 0.25s, display 0.25s allow-discrete;
      }

      .is-deleting {
        opacity: 0;
        height: 0;
        display: none;
        /* transform: skewX(50deg) translateX(-25vw); */
      }

      .modal {
        /* position: fixed; */
        top: 0;
        bottom: 0;
        height: 85vh;
        overflow-y: scroll;
        background-color: #151515;
        color: var(--fg);
        border-radius: 12px;
        border: 2px solid #303030;
        /* scrollbar */
        scrollbar-width: thin;
        transition: opacity 0.25s;
      }

      .model-modal {
        box-sizing: border-box;
        padding: 0 16px;
        width: 100%;
        display: flex;
        flex-direction: column;
        gap: 8px;
        width: 100%;

        > div {
          width: 100%;
          display: flex;
          flex-direction: column;
          justify-content: center;
        }
      }

      .disabled {
        opacity: 0.1;
        pointer-events: none;
        user-select: none;
      }

      .faded-text-button {
        cursor: pointer;
        font-size: small;
        color: gray;

        &:hover {
          text-decoration: underline;
        }
      }
    </style>
  </head>

  <body>
    <div>Please wait...</div>
  </body>

  <script type="module">
    // import { h, render } from "https://esm.sh/preact"
    // import { useState, useEffect } from "https://esm.sh/preact/hooks"
    // import htm from "https://esm.sh/htm"
    // const html = htm.bind(h)
    import {
      render,
      html,
      useState,
      useEffect,
    } from "data:text/javascript;charset=utf-8,var e,n,_,t,o,r,u,l={},i=[],c=/acit|ex(?:s|g|n|p|$)|rph|grid|ows|mnc|ntw|ine[ch]|zoo|^ord|itera/i;function s(e,n){for(var _ in n)e[_]=n[_];return e}function f(e){var n=e.parentNode;n&&n.removeChild(e)}function a(n,_,t){var o,r,u,l={};for(u in _)\"key\"==u?o=_[u]:\"ref\"==u?r=_[u]:l[u]=_[u];if(arguments.length>2&&(l.children=arguments.length>3?e.call(arguments,2):t),\"function\"==typeof n&&null!=n.defaultProps)for(u in n.defaultProps)void 0===l[u]&&(l[u]=n.defaultProps[u]);return p(n,l,o,r,null)}function p(e,t,o,r,u){var l={type:e,props:t,key:o,ref:r,__k:null,__:null,__b:0,__e:null,__d:void 0,__c:null,__h:null,constructor:void 0,__v:null==u?++_:u};return null!=n.vnode&&n.vnode(l),l}function h(e){return e.children}function d(e,n){this.props=e,this.context=n}function v(e,n){if(null==n)return e.__?v(e.__,e.__.__k.indexOf(e)+1):null;for(var _;n<e.__k.length;n++)if(null!=(_=e.__k[n])&&null!=_.__e)return _.__e;return\"function\"==typeof e.type?v(e):null}function y(e){var n,_;if(null!=(e=e.__)&&null!=e.__c){for(e.__e=e.__c.base=null,n=0;n<e.__k.length;n++)if(null!=(_=e.__k[n])&&null!=_.__e){e.__e=e.__c.base=_.__e;break}return y(e)}}function m(e){(!e.__d&&(e.__d=!0)&&t.push(e)&&!g.__r++||r!==n.debounceRendering)&&((r=n.debounceRendering)||o)(g)}function g(){for(var e;g.__r=t.length;)e=t.sort(function(e,n){return e.__v.__b-n.__v.__b}),t=[],e.some(function(e){var n,_,t,o,r,u;e.__d&&(r=(o=(n=e).__v).__e,(u=n.__P)&&(_=[],(t=s({},o)).__v=o.__v+1,P(u,o,t,n.__n,void 0!==u.ownerSVGElement,null!=o.__h?[r]:null,_,null==r?v(o):r,o.__h),D(_,o),o.__e!=r&&y(o)))})}function k(e,n,_,t,o,r,u,c,s,f){var a,d,y,m,g,k,x,H=t&&t.__k||i,E=H.length;for(_.__k=[],a=0;a<n.length;a++)if(null!=(m=_.__k[a]=null==(m=n[a])||\"boolean\"==typeof m?null:\"string\"==typeof m||\"number\"==typeof m||\"bigint\"==typeof m?p(null,m,null,null,m):Array.isArray(m)?p(h,{children:m},null,null,null):m.__b>0?p(m.type,m.props,m.key,null,m.__v):m)){if(m.__=_,m.__b=_.__b+1,null===(y=H[a])||y&&m.key==y.key&&m.type===y.type)H[a]=void 0;else for(d=0;d<E;d++){if((y=H[d])&&m.key==y.key&&m.type===y.type){H[d]=void 0;break}y=null}P(e,m,y=y||l,o,r,u,c,s,f),g=m.__e,(d=m.ref)&&y.ref!=d&&(x||(x=[]),y.ref&&x.push(y.ref,null,m),x.push(d,m.__c||g,m)),null!=g?(null==k&&(k=g),\"function\"==typeof m.type&&null!=m.__k&&m.__k===y.__k?m.__d=s=b(m,s,e):s=C(e,m,y,H,g,s),f||\"option\"!==_.type?\"function\"==typeof _.type&&(_.__d=s):e.value=\"\"):s&&y.__e==s&&s.parentNode!=e&&(s=v(y))}for(_.__e=k,a=E;a--;)null!=H[a]&&(\"function\"==typeof _.type&&null!=H[a].__e&&H[a].__e==_.__d&&(_.__d=v(t,a+1)),U(H[a],H[a]));if(x)for(a=0;a<x.length;a++)T(x[a],x[++a],x[++a])}function b(e,n,_){var t,o;for(t=0;t<e.__k.length;t++)(o=e.__k[t])&&(o.__=e,n=\"function\"==typeof o.type?b(o,n,_):C(_,o,o,e.__k,o.__e,n));return n}function C(e,n,_,t,o,r){var u,l,i;if(void 0!==n.__d)u=n.__d,n.__d=void 0;else if(null==_||o!=r||null==o.parentNode)e:if(null==r||r.parentNode!==e)e.appendChild(o),u=null;else{for(l=r,i=0;(l=l.nextSibling)&&i<t.length;i+=2)if(l==o)break e;e.insertBefore(o,r),u=r}return void 0!==u?u:o.nextSibling}function x(e,n,_){\"-\"===n[0]?e.setProperty(n,_):e[n]=null==_?\"\":\"number\"!=typeof _||c.test(n)?_:_+\"px\"}function H(e,n,_,t,o){var r;e:if(\"style\"===n)if(\"string\"==typeof _)e.style.cssText=_;else{if(\"string\"==typeof t&&(e.style.cssText=t=\"\"),t)for(n in t)_&&n in _||x(e.style,n,\"\");if(_)for(n in _)t&&_[n]===t[n]||x(e.style,n,_[n])}else if(\"o\"===n[0]&&\"n\"===n[1])r=n!==(n=n.replace(/Capture$/,\"\")),n=n.toLowerCase()in e?n.toLowerCase().slice(2):n.slice(2),e.l||(e.l={}),e.l[n+r]=_,_?t||e.addEventListener(n,r?S:E,r):e.removeEventListener(n,r?S:E,r);else if(\"dangerouslySetInnerHTML\"!==n){if(o)n=n.replace(/xlink[H:h]/,\"h\").replace(/sName$/,\"s\");else if(\"href\"!==n&&\"list\"!==n&&\"form\"!==n&&\"tabIndex\"!==n&&\"download\"!==n&&n in e)try{e[n]=null==_?\"\":_;break e}catch(e){}\"function\"==typeof _||(null!=_&&(!1!==_||\"a\"===n[0]&&\"r\"===n[1])?e.setAttribute(n,_):e.removeAttribute(n))}}function E(e){this.l[e.type+!1](n.event?n.event(e):e)}function S(e){this.l[e.type+!0](n.event?n.event(e):e)}function P(e,_,t,o,r,u,l,i,c){var f,a,p,v,y,m,g,b,C,x,H,E=_.type;if(void 0!==_.constructor)return null;null!=t.__h&&(c=t.__h,i=_.__e=t.__e,_.__h=null,u=[i]),(f=n.__b)&&f(_);try{e:if(\"function\"==typeof E){if(b=_.props,C=(f=E.contextType)&&o[f.__c],x=f?C?C.props.value:f.__:o,t.__c?g=(a=_.__c=t.__c).__=a.__E:(\"prototype\"in E&&E.prototype.render?_.__c=a=new E(b,x):(_.__c=a=new d(b,x),a.constructor=E,a.render=A),C&&C.sub(a),a.props=b,a.state||(a.state={}),a.context=x,a.__n=o,p=a.__d=!0,a.__h=[]),null==a.__s&&(a.__s=a.state),null!=E.getDerivedStateFromProps&&(a.__s==a.state&&(a.__s=s({},a.__s)),s(a.__s,E.getDerivedStateFromProps(b,a.__s))),v=a.props,y=a.state,p)null==E.getDerivedStateFromProps&&null!=a.componentWillMount&&a.componentWillMount(),null!=a.componentDidMount&&a.__h.push(a.componentDidMount);else{if(null==E.getDerivedStateFromProps&&b!==v&&null!=a.componentWillReceiveProps&&a.componentWillReceiveProps(b,x),!a.__e&&null!=a.shouldComponentUpdate&&!1===a.shouldComponentUpdate(b,a.__s,x)||_.__v===t.__v){a.props=b,a.state=a.__s,_.__v!==t.__v&&(a.__d=!1),a.__v=_,_.__e=t.__e,_.__k=t.__k,_.__k.forEach(function(e){e&&(e.__=_)}),a.__h.length&&l.push(a);break e}null!=a.componentWillUpdate&&a.componentWillUpdate(b,a.__s,x),null!=a.componentDidUpdate&&a.__h.push(function(){a.componentDidUpdate(v,y,m)})}a.context=x,a.props=b,a.state=a.__s,(f=n.__r)&&f(_),a.__d=!1,a.__v=_,a.__P=e,f=a.render(a.props,a.state,a.context),a.state=a.__s,null!=a.getChildContext&&(o=s(s({},o),a.getChildContext())),p||null==a.getSnapshotBeforeUpdate||(m=a.getSnapshotBeforeUpdate(v,y)),H=null!=f&&f.type===h&&null==f.key?f.props.children:f,k(e,Array.isArray(H)?H:[H],_,t,o,r,u,l,i,c),a.base=_.__e,_.__h=null,a.__h.length&&l.push(a),g&&(a.__E=a.__=null),a.__e=!1}else null==u&&_.__v===t.__v?(_.__k=t.__k,_.__e=t.__e):_.__e=w(t.__e,_,t,o,r,u,l,c);(f=n.diffed)&&f(_)}catch(e){_.__v=null,(c||null!=u)&&(_.__e=i,_.__h=!!c,u[u.indexOf(i)]=null),n.__e(e,_,t)}}function D(e,_){n.__c&&n.__c(_,e),e.some(function(_){try{e=_.__h,_.__h=[],e.some(function(e){e.call(_)})}catch(e){n.__e(e,_.__v)}})}function w(n,_,t,o,r,u,i,c){var s,a,p,h=t.p)773ff122" 
R"773ff122(rops,d=_.props,y=_.type,m=0;if(\"svg\"===y&&(r=!0),null!=u)for(;m<u.length;m++)if((s=u[m])&&(s===n||(y?s.localName==y:3==s.nodeType))){n=s,u[m]=null;break}if(null==n){if(null===y)return document.createTextNode(d);n=r?document.createElementNS(\"http://www.w3.org/2000/svg\",y):document.createElement(y,d.is&&d),u=null,c=!1}if(null===y)h===d||c&&n.data===d||(n.data=d);else{if(u=u&&e.call(n.childNodes),a=(h=t.props||l).dangerouslySetInnerHTML,p=d.dangerouslySetInnerHTML,!c){if(null!=u)for(h={},m=0;m<n.attributes.length;m++)h[n.attributes[m].name]=n.attributes[m].value;(p||a)&&(p&&(a&&p.__html==a.__html||p.__html===n.innerHTML)||(n.innerHTML=p&&p.__html||\"\"))}if(function(e,n,_,t,o){var r;for(r in _)\"children\"===r||\"key\"===r||r in n||H(e,r,null,_[r],t);for(r in n)o&&\"function\"!=typeof n[r]||\"children\"===r||\"key\"===r||\"value\"===r||\"checked\"===r||_[r]===n[r]||H(e,r,n[r],_[r],t)}(n,d,h,r,c),p)_.__k=[];else if(m=_.props.children,k(n,Array.isArray(m)?m:[m],_,t,o,r&&\"foreignObject\"!==y,u,i,u?u[0]:t.__k&&v(t,0),c),null!=u)for(m=u.length;m--;)null!=u[m]&&f(u[m]);c||(\"value\"in d&&void 0!==(m=d.value)&&(m!==n.value||\"progress\"===y&&!m)&&H(n,\"value\",m,h.value,!1),\"checked\"in d&&void 0!==(m=d.checked)&&m!==n.checked&&H(n,\"checked\",m,h.checked,!1))}return n}function T(e,_,t){try{\"function\"==typeof e?e(_):e.current=_}catch(e){n.__e(e,t)}}function U(e,_,t){var o,r;if(n.unmount&&n.unmount(e),(o=e.ref)&&(o.current&&o.current!==e.__e||T(o,null,_)),null!=(o=e.__c)){if(o.componentWillUnmount)try{o.componentWillUnmount()}catch(e){n.__e(e,_)}o.base=o.__P=null}if(o=e.__k)for(r=0;r<o.length;r++)o[r]&&U(o[r],_,\"function\"!=typeof e.type);t||null==e.__e||f(e.__e),e.__e=e.__d=void 0}function A(e,n,_){return this.constructor(e,_)}function M(_,t,o){var r,u,i;n.__&&n.__(_,t),u=(r=\"function\"==typeof o)?null:o&&o.__k||t.__k,i=[],P(t,_=(!r&&o||t).__k=a(h,null,[_]),u||l,l,void 0!==t.ownerSVGElement,!r&&o?[o]:u?null:t.firstChild?e.call(t.childNodes):null,i,!r&&o?o:u?u.__e:t.firstChild,r),D(i,_)}function F(e,n){var _={__c:n=\"__cC\"+u++,__:e,Consumer:function(e,n){return e.children(n)},Provider:function(e){var _,t;return this.getChildContext||(_=[],(t={})[n]=this,this.getChildContext=function(){return t},this.shouldComponentUpdate=function(e){this.props.value!==e.value&&_.some(m)},this.sub=function(e){_.push(e);var n=e.componentWillUnmount;e.componentWillUnmount=function(){_.splice(_.indexOf(e),1),n&&n.call(e)}}),e.children}};return _.Provider.__=_.Consumer.contextType=_}e=i.slice,n={__e:function(e,n){for(var _,t,o;n=n.__;)if((_=n.__c)&&!_.__)try{if((t=_.constructor)&&null!=t.getDerivedStateFromError&&(_.setState(t.getDerivedStateFromError(e)),o=_.__d),null!=_.componentDidCatch&&(_.componentDidCatch(e),o=_.__d),o)return _.__E=_}catch(n){e=n}throw e}},_=0,d.prototype.setState=function(e,n){var _;_=null!=this.__s&&this.__s!==this.state?this.__s:this.__s=s({},this.state),\"function\"==typeof e&&(e=e(s({},_),this.props)),e&&s(_,e),null!=e&&this.__v&&(n&&this.__h.push(n),m(this))},d.prototype.forceUpdate=function(e){this.__v&&(this.__e=!0,e&&this.__h.push(e),m(this))},d.prototype.render=h,t=[],o=\"function\"==typeof Promise?Promise.prototype.then.bind(Promise.resolve()):setTimeout,g.__r=0,u=0;var L,N,W,R=0,I=[],O=n.__b,V=n.__r,q=n.diffed,B=n.__c,$=n.unmount;function j(e,_){n.__h&&n.__h(N,e,R||_),R=0;var t=N.__H||(N.__H={__:[],__h:[]});return e>=t.__.length&&t.__.push({}),t.__[e]}function G(e){return R=1,z(ie,e)}function z(e,n,_){var t=j(L++,2);return t.t=e,t.__c||(t.__=[_?_(n):ie(void 0,n),function(e){var n=t.t(t.__[0],e);t.__[0]!==n&&(t.__=[n,t.__[1]],t.__c.setState({}))}],t.__c=N),t.__}function J(e,_){var t=j(L++,3);!n.__s&&le(t.__H,_)&&(t.__=e,t.__H=_,N.__H.__h.push(t))}function K(e,_){var t=j(L++,4);!n.__s&&le(t.__H,_)&&(t.__=e,t.__H=_,N.__h.push(t))}function Q(e){return R=5,Y(function(){return{current:e}},[])}function X(e,n,_){R=6,K(function(){\"function\"==typeof e?e(n()):e&&(e.current=n())},null==_?_:_.concat(e))}function Y(e,n){var _=j(L++,7);return le(_.__H,n)&&(_.__=e(),_.__H=n,_.__h=e),_.__}function Z(e,n){return R=8,Y(function(){return e},n)}function ee(e){var n=N.context[e.__c],_=j(L++,9);return _.c=e,n?(null==_.__&&(_.__=!0,n.sub(N)),n.props.value):e.__}function ne(e,_){n.useDebugValue&&n.useDebugValue(_?_(e):e)}function _e(e){var n=j(L++,10),_=G();return n.__=e,N.componentDidCatch||(N.componentDidCatch=function(e){n.__&&n.__(e),_[1](e)}),[_[0],function(){_[1](void 0)}]}function te(){I.forEach(function(e){if(e.__P)try{e.__H.__h.forEach(re),e.__H.__h.forEach(ue),e.__H.__h=[]}catch(_){e.__H.__h=[],n.__e(_,e.__v)}}),I=[]}n.__b=function(e){N=null,O&&O(e)},n.__r=function(e){V&&V(e),L=0;var n=(N=e.__c).__H;n&&(n.__h.forEach(re),n.__h.forEach(ue),n.__h=[])},n.diffed=function(e){q&&q(e);var _=e.__c;_&&_.__H&&_.__H.__h.length&&(1!==I.push(_)&&W===n.requestAnimationFrame||((W=n.requestAnimationFrame)||function(e){var n,_=function(){clearTimeout(t),oe&&cancelAnimationFrame(n),setTimeout(e)},t=setTimeout(_,100);oe&&(n=requestAnimationFrame(_))})(te)),N=void 0},n.__c=function(e,_){_.some(function(e){try{e.__h.forEach(re),e.__h=e.__h.filter(function(e){return!e.__||ue(e)})}catch(t){_.some(function(e){e.__h&&(e.__h=[])}),_=[],n.__e(t,e.__v)}}),B&&B(e,_)},n.unmount=function(e){$&&$(e);var _=e.__c;if(_&&_.__H)try{_.__H.__.forEach(re)}catch(e){n.__e(e,_.__v)}};var oe=\"function\"==typeof requestAnimationFrame;function re(e){var n=N;\"function\"==typeof e.__c&&e.__c(),N=n}function ue(e){var n=N;e.__c=e.__(),N=n}function le(e,n){return!e||e.length!==n.length||n.some(function(n,_){return n!==e[_]})}function ie(e,n){return\"function\"==typeof n?n(e):n}var ce=function(e,n,_,t){var o;n[0]=0;for(var r=1;r<n.length;r++){var u=n[r++],l=n[r]?(n[0]|=u?1:2,_[n[r++]]):n[++r];3===u?t[0]=l:4===u?t[1]=Object.assign(t[1]||{},l):5===u?(t[1]=t[1]||{})[n[++r]]=l:6===u?t[1][n[++r]]+=l+\"\":u?(o=e.apply(l,ce(e,l,_,[\"\",null])),t.push(o),l[0]?n[0]|=2:(n[r-2]=0,n[r]=o)):t.push(l)}return t},se=new Map,fe=function(e){var n=se.get(this);return n||(n=new Map,se.set(this,n)),(n=ce(this,n.get(e)||(n.set(e,n=function(e){for(var n,_,t=1,o=\"\",r=\"\",u=[0],l=function(e){1===t&&(e||(o=o.replace(/^\\s*\\n\\s*|\\s*\\n\\s*$/g,\"\")))?u.push(0,e,o):3===t&&(e||o)?(u.push(3,e,o),t=2):2===t&&\"...\"===o&&e?u.push(4,e,0):2===t&&o&&!e?u.push(5,0,!0,o):t>=5&&((o||!e&&5===t)&&(u.push(t,0,o,_),t=6),e&&(u.push(t,e,0,_),t=6)),o=\"\"},i=0;i<e.length;i++){i&&(1===t&&l(),l(i));for(var c=0;c<e[i].length;c++)n=e[i][c],1===t?\"<\"===n?(l(),u=[u],t=3):o+=n:4===t?\"--\"===o&&\">\"===n?(t=1,o=\"\"):o=n+o[0]:r?n===r?r=\"\":o+=n:'\"'===n||\"'\"===n?r=n:\">\"===n?(l(),t=1):t&&(\"=\"===n?(t=5,_=o,o=\"\"):\"/\"===n&&(t<5||\">\"===e[i][c+1])?(l(),3===t&&(u=u[0]),t=u,(u=u[0]).push(2,0,t),t=0):\" \"===n||\"\\t\"===n||\"\\n\"===n||\"\\r\"===n?(l(),t=2):o+=n),3===t&&\"!--\"===o&&(t=4,u=u[0])}return l(),u}(e)),n),arguments,[])).length>1?n:n[0]}.bind(a);export{a as h,fe as html,M as render,d as Component,F as createContext,G as useState,z as useReducer,J as useEffect,K as useLayoutEffect,Q as useRef,X as useImperativeHandle,Y as useMemo,Z as useCallback,ee as useContext,ne as useDebugValue,_e as useErrorBoundary};\n"
    const isServerRunningLocally = ["localhost", "127.0.0.1", "[::1]", ""].includes(
      location.hostname
    )
    const modelFileExtensions = [".ckpt", ".safetensors", ".ggml", ".gguf"]

    async function openFilePickerDialogue() {
      const ret = await window.showOpenFilePicker({
        types: [
          {
            description: "Model files",
            accept: {
              "application/octet-stream": modelFileExtensions,
            },
          },
        ],
        excludeAcceptAllOption: true,
        multiple: false,
      })
      console.log("Files", ret)
      if (!ret || !ret.length) return
      const first = await ret[0].getFile()
      return { name: first.name, size: first.size }
    }
    window.openFilePickerDialogue = openFilePickerDialogue

    function Slider({ label, min, max, defaultValue, value, setValue, style, type = "text" }) {
      return html`
        <div style=${style}>
          <div class="label">${label}</div>
          <div class="slider">
            <input
              type="range"
              min=${min}
              max=${max}
              value=${value}
              onInput=${(e) => setValue(e.target.value)} />
            ${defaultValue
              ? html`
                  <button title="Reset to default" onClick=${() => setValue(defaultValue || 0)}>
                    🔁
                  </button>
                `
              : null}
            <input
              type=${type}
              value=${value}
              style="width: 50px;text-align:center;"
              onInput=${(e) => setValue(e.target.value)} />
          </div>
        </div>
      `
    }

    function Select({ label, options, value, setValue }) {
      return html`
        <div style="margin: 8px 0px;">
          <div class="label">${label}</div>
          <select style="width:100%;" value=${value} onChange=${(e) => setValue(e.target.value)}>
            ${options.map((option) => html`<option value=${option.value}>${option.label}</option>`)}
          </select>
        </div>
      `
    }

    function Switch({ label, value, setValue }) {
      return html`
        <div style="margin: 8px 0px; font-weight: bold;">
          <label class="switch">
            <input type="checkbox" checked=${value} onChange=${(e) => setValue(e.target.checked)} />
            <span class="label">${label}</span>
          </label>
        </div>
      `
    }

    function TextInput({ label, value, setValue }) {
      return html`
        <div>
          <div class="label">${label}</div>
          <input type="text" value=${value} onInput=${(e) => setValue(e.target.value)} />
        </div>
      `
    }

    let currentGenerationOptions = {}

    function GenerationInput({ mode }) {
      const samplingMethods = [
        ["EULER_A", "Euler Ancestral"],
        ["EULER", "Euler"],
        ["HEUN", "Heun"],
        ["DPM2", "DPM2"],
        ["DPMPP2S_A", "DPM++ (2S) Ancestral"],
        ["DPMPP2S", "DPM++ (2S)"],
        ["DPMPP2Sv2", "DPM++ (2S) (New)"],
        ["DPMPP2M_A", "DPM++ (2M) Ancestral"],
        ["DPMPP2M", "DPM++ (2M)"],
        ["DPMPP2Mv2", "DPM++ (2M) (New)"],
        ["LCM", "Latent Consistency Models"],
      ]
      const defaultWidth = 512
      const defaultHeight = 512
      const [width, setWidth] = useState(defaultWidth)
      const [height, setHeight] = useState(defaultHeight)
      const [image, setImage] = useState(null)
      const [prompt, setPrompt] = useState("")
      const [negativePrompt, setNegativePrompt] = useState("")
      const [batchSize, setBatchSize] = useState(1)
      const [batchCount, setBatchCount] = useState(1)
      const [seed, setSeed] = useState(0)
      const [samplingSteps, setSamplingSteps] = useState(20)
      const [showAdvanced, setShowAdvanced] = useState(false)
      const [autoSave, setAutoSave] = useState(true)
      const [samplingMethod, setSamplingMethod] = useState(samplingMethods[0][0])
      const [upscaleRuns, setUpscaleRuns] = useState(0)
      const [controlStrength, setControlStrength] = useState(0.9)
      const [styleRatio, setStyleRatio] = useState(20)
      const [cfgScale, setCfgScale] = useState(7)
      const [denoisingStrength, setDenoisingStrength] = useState(0.75)
      const [clipSkip, setClipSkip] = useState(0)

      Object.assign(currentGenerationOptions, {
        mode,
        width,
        height,
        image,
        prompt,
        "negative-prompt": negativePrompt,
        "batch-size": batchSize,
        "batch-count": batchCount,
        seed,
        steps: samplingSteps,
        // showAdvanced,
        "auto-save": Number(autoSave),
        "sampling-method": samplingMethod,
        // upscaleRuns,
        "control-strength": controlStrength,
        "style-ratio": styleRatio,
        "cfg-scale": cfgScale,
        "denoising-strength": denoisingStrength,
        "clip-skip": clipSkip,
      })

      function onImageSelect(e) {
        const file = e.target.files[0]
        if (!file) return
        const reader = new FileReader()
        reader.onload = (e) => {
          setImage(e.target.result)
        }
        reader.readAsDataURL(file)
      }

      window.setRefImageExt = (img) => {
        setImage(img)
        const el = document.getElementById("image-upload")
        if (el) el.value = ""
      }

      return html`<div class="framed" style="min-width: 40%;">
        <div class="subtitle">${mode === "txt2img" ? "Text Input" : "Image Input"}</div>
        <div class="gen-options">
          ${mode === "img2img"
            ? html` <div>
                <div class="label">Reference image</div>
                <div style="text-align:center;margin-top:6px;">
                  ${image ? html`<img id="image-preview" src=${image} alt="" />` : null}
                </div>
                <div
                  style="margin-top: 6px;display:flex;justify-content: space-between;align-items: center;">
                  <input type="file" id="image-upload" accept="image/*" onChange=${onImageSelect} />
                  <button
                    id="upload"
                    onClick=${() => {
                      window.setRefImageExt(null)
                    }}>
                    Clear
                  </button>
                </div>
              </div>`
            : null}
          <div>
            <div class="label">Positive prompt</div>
            <textarea
              name=""
              id=""
              cols="30"
              rows="10"
              placeholder="Description of desired image and elements"
              onInput=${(e) => setPrompt(e.target.value)}
              value=${prompt}></textarea>
          </div>
          <div>
            <div class="label">Negative prompt</div>
            <textarea
              name=""
              id=""
              cols="30"
              rows="10"
              placeholder="Optional description of undesired image details"
              onInput=${(e) => setNegativePrompt(e.target.value)}
              value=${negativePrompt}></textarea>
          </div>
          <${Slider}
            label="Sampling Steps"
            min="1"
            max="128"
            type="number"
            value=${samplingSteps}
            defaultValue=${20}
            setValue=${setSamplingSteps} />
          <div class="grid" style="grid-template-columns: 20% 80%">
            <div class="opt-save">
              <label>
                <input
                  type="checkbox"
                  checked=${autoSave}
                  onChange=${(e) => setAutoSave(e.target.checked)} />
                <div style="font-weight:bold;">Auto Save</div>
              </label>
            </div>
            <${Slider}
              label="Image count"
              min="1"
              max="32"
              value=${batchCount}
              setValue=${setBatchCount} />
          </div>
          <div
            style="display: flex; justify-content: center; align-items: center; margin-top: 8px; cursor: pointer;color:cyan;"
            onClick=${() => setShowAdvanced(!showAdvanced)}>
            ${showAdvanced ? "Hide" : "Show"} generation options
          </div>
          <div id="more-options" class=${showAdvanced ? "" : "is-deleting"}>
            <${Slider}
              label="Width"
              min="0"
              max="2048"
              value=${width}
              setValue=${setWidth}
              defaultValue=${defaultWidth} />
            <${Slider}
              label="Height"
              min="0"
              max="2048"
              value=${height}
              setValue=${setHeight}
              defaultValue=${defaultHeight} />
            <!--<div class="group">
              <${Slider}
                label="Batch size"
                min="1"
                max="32"
                value=${batchSize}
                setValue=${setBatchSize} />
            </div>-->
            <di)773ff122" 
R"773ff122(v class="group">
              <div>
                <div class="label">Seed</div>
                <input
                  type="text"
                  value=${seed}
                  onInput=${(e) => setSeed(e.target.value)}
                  style="width: calc(100% - 128px - 24px);" />
                <button
                  title="-1 will create a random seed on each generation"
                  onClick=${() => {
                    setSeed(-1)
                  }}>
                  🎲
                </button>
                <button
                  title="Zero seed for deterministic generation"
                  style="margin-left: 6px"
                  onClick=${() => {
                    setSeed(0)
                  }}>
                  0️⃣
                </button>
              </div>
              <${Slider}
                label="Guidance (CFG) Scale"
                min="1"
                max="20"
                value=${cfgScale}
                setValue=${setCfgScale} />
            </div>
            <div class="group">
              <${Select}
                label="Sampling method"
                options=${samplingMethods.map((e) => ({ label: e[1], value: e[0] }))}
                value=${samplingMethod}
                setValue=${setSamplingMethod} />
              <${Slider}
                label="Upscaler runs"
                min="0"
                max="32"
                value=${upscaleRuns}
                setValue=${setUpscaleRuns} />
            </div>
            <div class="group">
              <${Slider}
                label="Style Ratio (%)"
                min="0"
                max="100"
                value=${styleRatio}
                setValue=${setStyleRatio} />
              <${Slider}
                label="Control Strength"
                min="0"
                max="1"
                value=${controlStrength}
                setValue=${setControlStrength} />
            </div>
            ${mode === "txt2img"
              ? html`<${Slider}
                  label="Clip Skip"
                  min="0"
                  max="32"
                  value=${clipSkip}
                  setValue=${setClipSkip} />`
              : html`<div class="group">
                  <${Slider}
                    label="Clip Skip"
                    min="0"
                    max="32"
                    value=${clipSkip}
                    setValue=${setClipSkip} />
                  <${Slider}
                    label="Denoising Strength"
                    min="0"
                    max="1"
                    value=${denoisingStrength}
                    setValue=${setDenoisingStrength} />
                </div>`}
          </div>
        </div>
      </div>`
      //               <${Switch} label="Don't save" value=${false} setValue=${console.log} />
    }

    function ImgOutput({ width, height, outputStatus, setActiveView }) {
      const images = outputStatus.images || []
      function onDownloadClick(ix = 0) {
        const a = document.createElement("a")
        a.href = images[ix].src
        a.download = "generated.png"
        a.click()
      }

      function setImageAsReference(src) {
        window.setRefImageExt(src)
        setActiveView("img2img")
      }

      return html`
        <div class="framed">
          <div class="subtitle">Image output</div>
          ${images.map(
            (img, i) =>
              html`<div style="margin: 8px;">
                  <img
                    src=${img.src}
                    alt=""
                    style="max-width: 100%; border: 1px solid #303030; border-radius: 4px;" />
                </div>
                <div class="buttons gap">
                  <button
                    class="btn-blue"
                    onClick=${() => {
                      setImageAsReference(img.src)
                    }}>
                    Use as reference
                  </button>
                  <button onClick=${() => onDownloadClick(i)}>Download</button>
                </div>`
          )}
          ${outputStatus.progress
            ? html`<div class="rainbow">${outputStatus.progress}</div>`
            : null}
          ${!outputStatus.progress && outputStatus.ok && !images.length
            ? html`<p style="margin:2vw;text-align:center;">
                Ready to generate an image!<br /><br />Your image will appear here after it's
                generated.
              </p>`
            : null}
          ${outputStatus.ok
            ? null
            : html`<p>Sorry, we couldn't generate an image right now.</p>
                <p>Please ensure that you're using the latest version of SDUI.</p>
                <p style="color:orange;">Server returned: ${outputStatus.message}</p>`}
        </div>
      `
    }

    function ContentBar({
      models,
      activeView,
      setActiveView,
      updateActiveModel,
      refreshModels,
      modelStatus,
    }) {
      async function onSelectModel() {
        const file = await openFilePickerDialogue()
      }
      return html`<div class="bar">
        <div style="display: flex; align-items: flex-end">
          <div>
            <div style="margin-bottom:6px;">
              <span style="font-weight:bold; font-size: small;">Set Model</span>
            </div>
            <select name="model-selector" id="model-selector" onChange=${updateActiveModel}>
              <option disabled selected>Select model checkpoint</option>
              ${models.map((model) => html`<option value=${model.value}>${model.label}</option>`)}
              <!--<option value="1">sd-v1-5-pruned-emaonly.safetensors</option>
              <option>ponyRealism_v21VAE.safetensors</option>-->
            </select>
          </div>
          <div>
            <div style="margin-bottom:6px;">
              ${modelStatus.ok
                ? null
                : html`<span style="font-size: small;color:orange;">${modelStatus.message}</span>`}
            </div>
            <button onClick=${refreshModels}>Refresh Models</button>
          </div>
          <!--<button style="margin-left:6px;">Convert to GGUF</button>-->
          ${isServerRunningLocally && false
            ? html`<button class="btn-primary" onClick=${onSelectModel}>Import model...</button>`
            : null}
          <!--<button class="btn-primary" style="margin-left:6px;">Get models</button>-->
        </div>
        <div class="buttons gap">
          <button
            class=${activeView === "txt2img" ? "btn-warn" : ""}
            onClick=${() => setActiveView("txt2img")}>
            Image from text
          </button>
          <button
            class=${activeView === "img2img" ? "btn-warn" : ""}
            onClick=${() => setActiveView("img2img")}>
            Image from image
          </button>
        </div>
      </div>`
    }

    function ContentSubmissionBar({ openModelOptsModal, submitRequest }) {
      return html`<div class="buttons" style="display: flex; justify-content: center; gap: 6px">
        <button
          class="btn-warn"
          onClick=${() => {
            submitRequest(currentGenerationOptions)
          }}>
          Generate
        </button>
        <!--<button>View History</button>-->
        <button
          id="btn-model-options"
          onClick=${() => {
            openModelOptsModal()
            const el = document.getElementById("btn-model-options")
            el.style.visibility = "hidden"
            setTimeout(() => {
              el.style.visibility = null
            }, 100)
          }}>
          Model Options
        </button>
      </div>`
      //         ${isServerRunningLocally ? html`<button>Open Outputs</button>` : null}
    }

    function Content({
      models,
      updateActiveModel,
      refreshModels,
      modelStatus,
      outputStatus,
      submitRequest,
    }) {
      const [activeView, setActiveView] = useState("txt2img" /* or "img2img" */)
      const [showModelOptions, setShowModelOptions] = useState(false)

      // console.log("Model Status", modelStatus)

      return html`<div class="content">
        ${showModelOptions
          ? html`<${ModelOptionsDialog} close=${() => setShowModelOptions(false)} />`
          : null}
        <${ContentBar}
          models=${models}
          updateActiveModel=${updateActiveModel}
          refreshModels=${refreshModels}
          modelStatus=${modelStatus}
          activeView=${activeView}
          setActiveView=${setActiveView} />
        <hr />
        ${!modelStatus.model && !modelStatus.justStarted
          ? html`<progress id="progress-bar" aria-label="Content loading…"></progress>`
          : ""}
        <div class=${!modelStatus.model && !modelStatus.justStarted ? "disabled" : ""}>
          ${modelStatus.justStarted
            ? html`<div>
                <h2 style="margin-left:2%;">👆 Select a Model</h2>
                <h4 style="text-align:center;">
                  Added a model to your <code>--filesDir</code> but don't see it?<br /><br />Click
                  "Refresh Models" to recursively find all .gguf, .safetensors, and .ckpt files
                  inside the following specified directories:
                </h4>
                <div class="hello-message">
                  ${FILES_DIR.map((dir) => html`<div><code>${dir}</code></div>`)}
                </div>
              </div>`
            : html`<div class="io">
                <${GenerationInput} mode=${activeView} />
                <${ImgOutput}
                  width=${512}
                  height=${512}
                  outputStatus=${outputStatus}
                  setActiveView=${setActiveView} />
              </div>`}
          <!--<div class="title">Image from image</div>-->
        </div>
        ${modelStatus.model
          ? html`<${ContentSubmissionBar}
              openModelOptsModal=${() => setShowModelOptions(true)}
              submitRequest=${submitRequest} />`
          : null}
      </div>`
    }

    function Header() {
      return html`<div class="header">
        <div>
          <div class="title">
            Stable Diffusion <span style="font-size:medium;color:lightgray;">.cpp</span>
          </div>
        </div>
        <div style="display: flex; gap: 16px">
          <div></div>
          <div>API doc</div>
          <div>
            <a target="_blank" href="https://github.com/extremeheat/stable-diffusion.cpp">GitHub</a>
          </div>
        </div>
      </div>`
    }

    function InputFilePicker({ label, placeholder, value, setValue }) {
      return html`<div style="font-weight: bold; margin: 4px 0;width: calc(256px + 500px + 32px);">
        <label for="vae">${label}</label>
        <div style="margin:4px 0;display: flex;align-items:center;">
          <input type="text" placeholder=${placeholder} value=${value} style="width: 500px;" />
          <!--<input type="file" style="width: 256px;" />-->
        </div>
      </div>`
    }

    function SelectFilePicker({ label, options, value, setValue }) {
      return html`<div style="margin: 8px 0px; width: calc(256px + 500px + 32px);">
        <div class="label">${label}</div>
        <div style="margin:4px 0;display: flex;align-items:center;">
          <select style="width:400px" value=${value} onChange=${(e) => setValue(e.target.value)}>
            ${options.map((option) => html`<option value=${option.value}>${option.label}</option>`)}
          </select>
          <!--<input type="file" style="width: 256px;" />-->
        </div>
      </div>`
    }

    function ModelOptionsDialog({ close }) {
      // These are the options we need to pass to the model:
      // --vae [VAE]                        path to vae
      // --taesd [TAESD_PATH]               path to taesd. Using Tiny AutoEncoder for fast decoding (low quality)
      // --control-net [CONTROL_PATH]       path to control net model
      // --embd-dir [EMBEDDING_PATH]        path to embeddings.
      // --stacked-id-embd-dir [DIR]        path to PHOTOMAKER stacked id embeddings.
      // --input-id-images-dir [DIR]        path to PHOTOMAKER input id images dir.
      // --normalize-input                  normalize PHOTOMAKER input id images
      // --upscale-model [ESRGAN_PATH]      path to esrgan model. Upscale images after generate, just RealESRGAN_x4plus_anime_6B supported by now.
      // The ones that require paths, we can use a file picker to select the file.

      const dummyOpts = []
      const pickableModels = [
        { label: "Upscaler Model", tooltip: "Path to ESRGAN model", value: "esrgan" },
        { label: "VAE Model", tooltip: "Path to VAE model", value: "vae" },
        {
          label: "Tiny AutoEncoder (TAESD) Model",
          tooltip: "Path to Tiny AutoEncoder",
          value: "taesd",
        },
        { label: "Control Net Model", tooltip: "Path to Control Net model", value: "control" },
        {
          label: "Embedding Model",
          tooltip: "Path to Embedding directory",
          value: "embeddings",
          type: "dir",
        },
        {
          label: "PhotoMaker: Stacked ID Embedding Model",
          tooltip: "Path to Stacked ID Embedding model",
          value: "stackedIdEmbeddings",
        },
        {
          label: "PhotoMaker: Input ID Images Directory",
          tooltip: "Path to Input ID Images directory",
          value: "inputIdImages",
          type: "dir",
        },
      ]
      return html`<dialog
        class="modal"
        style="width: 810px; height: fit-content;"
        open
        onCancel=${close}>
        <h3 style="text-align:center;">Model Options</h3>
        <div class="model-modal">
          ${pickableModels.map((model) =>
            model.type === "dir"
              ? "" ??
                html`<div>
                  <${InputFilePicker} label=${model.label} placeholder=${model.tooltip} />
                </div>`
              : html`<div>
                  <${SelectFilePicker}
                    label=${model.label}
                    options=${dummyOpts}
                    value=""
                    setValue=${console.log} />
                </div>`
          )}
          <${Switch} label="PhotoMaker: Normalize Input" value=${false} setValue=${console.log} />
        </div>
        <div style="display: flex; justify-content: center; gap: 16px;">
          <button class="btn-warn" onclick=${close}>OK</button>
        </div>
      </dialog>`
    }

    const API_HOST = window.API_HOST ?? "http://localhost:8024"
    const API_PATH = window.API_PATH ?? "/api/v0"
    const FILES_DIR = (window.FILES_DIR ?? "C:/Users/Something/someDesktop/test").split("*")
    const OUTPUTS_DIR = window.OUTPUTS_DIR ?? FILES_DIR[0]
    const IS_MOCK = API_HOST.includes("localhost")

    const RATELIMIT_RETRY_TIME = 5000
    const STATUS_CHECK_MAX = 1_000 // How many seconds to wait for a job to complete before giving up
    const STATUS_CHECK_INTERVAL = 1000 // How often to check for job completion
    const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms))

    let _generationMockResponse = {}
    function mockFetch(url) {
      async function getTestB64() {
        const img = await fetch(
          "https://images.pexels.com/photos/45201/kitty-cat-kitten-pet-45201.jpeg"
        ).then((r) => r.blob())
        const reader = new FileReader()
        reader.readAsDataURL(img)
        return new Promise((resolve) => {
          reader.onloadend = () => resolve(reader.result.split(",")[1])
        })
      }
      if (url.includes("/api/v0/models")) {
        return Promise.resolve({
          ok: true,
          text: () => Promise.resolve("39117 bundle.ggml\n22 someOther.safetensors"),
        })
      } else if (url.includes("/api/v0/prepare/")) {
        console.log("Preparing model", url.split("/").pop())
        return new Promise((resolve) =>
          setTimeout(() => resolve({ ok: true, te)773ff122" 
R"773ff122(xt: () => Promise.resolve("OK") }), 1000)
        )
      } else if (url.includes("/api/v0/txt2img")) {
        const job = Math.random().toString(36).substring(7)
        return Promise.resolve({
          ok: true,
          text() {
            _generationMockResponse[job] = "PENDING\n"
            setTimeout(async () => {
              _generationMockResponse[job] = "SUCCESS\n1\n" + (await getTestB64())
              setTimeout(() => {
                _generationMockResponse[job] = "404 Not Found"
              }, 5000)
            }, 5000)
            return Promise.resolve("OK\n" + job)
          },
        })
      } else if (url.includes("/api/v0/check/")) {
        const id = url.split("/").pop()
        return Promise.resolve({
          ok: true,
          text: () => Promise.resolve(_generationMockResponse[id] || "404 Not Found"),
        })
      } else {
        return Promise.resolve({
          ok: false,
          text: () => Promise.resolve("404"),
        })
      }
    }

    const generationTimes = []
    window.averageGenerationTime = 0
    function updateAverageGenerationTime() {
      window.averageGenerationTime =
        generationTimes.reduce((a, b) => a + b, 0) / generationTimes.length
    }

    let activeModel

    function App() {
      const [models, setModels] = useState([])
      const [modelStatus, setModelStatus] = useState({ justStarted: true, ok: false, message: "" })
      // const [modelStatus, setModelStatus] = useState({ justStarted: false, ok: true, message: "" })
      const [outputStatus, setOutputStatus] = useState({ ok: true, message: "" })

      const request = IS_MOCK ? mockFetch : fetch
      async function apiCall(endpoint, postBody, statusCb) {
        try {
          const payload = {
            method: postBody ? "POST" : "GET",
            body: postBody,
            headers: {
              "Content-Type": "text/plain",
            },
          }
          const response = await request(API_HOST + API_PATH + endpoint, payload).then((e) =>
            e.text()
          )
          const [code, ...rest] = response.split("\n")
          // console.log("API Call Response to", endpoint, "with", postBody, code, rest)
          if (code === "BUSY") {
            statusCb?.(true, "Rate limited") // Try again after 5 seconds
            setTimeout(() => apiCall(endpoint, statusCb), RATELIMIT_RETRY_TIME)
          }
          return [code, rest]
        } catch (e) {
          console.error(e)
          alert("Network error. Please check your connection and try again.")
        }
      }

      async function loadModels() {
        const avaliableModels = await apiCall("/models")
        setModels(
          avaliableModels[1]
            .filter((e) => !!e)
            .map((e) => {
              const [size, name] = e.split(" ")
              return { label: name, value: name }
            })
        )
      }

      useEffect(async () => {
        await loadModels()
      }, [])

      async function refreshModels() {
        const [code, [message]] = await apiCall("/refresh")
        if (code === "OK") {
          await loadModels()
        } else {
          setModelStatus({ ok: false, message: "Failed to refresh models! " + message })
        }
      }

      async function setActiveModel(value) {
        setModelStatus({ ok: false, message: "Preparing..." })
        const [code, [message]] = await apiCall(
          `/prepare/${value.replace(" ", "%20")}`,
          null,
          (busy) => setModelStatus({ ok: false, message: busy ? "Pending..." : "Preparing..." })
        )
        if (code === "OK") {
          setModelStatus({ ok: true, model: value, message: "Model prepared successfully" })
          activeModel = value
        } else {
          setModelStatus({ ok: false, message: message })
        }
      }

      async function onModelChange(e) {
        console.log("Selected model", e.target.value)
        try {
          return setActiveModel(e.target.value)
        } catch (e) {
          console.error(e)
          alert(
            "There was an error loading the specified model. Please check your network connection and try again."
          )
        }
      }

      async function watchGenerationJob(id, data) {
        const start = Date.now()
        for (let i = 0; i < STATUS_CHECK_MAX; i++) {
          const [code, generations] = await apiCall(`/check/${id}`)

          const images = []
          if ((code === "SUCCESS" || code === "PENDING") && generations.length) {
            const metadata = generations.shift()
            const output = { ok: true, message: "OK", images }
            let seedOffset = 0
            for (const generation of generations) {
              const [image] = generation.split(" ")
              const src = "data:image/png;base64," + image
              images.push({ src, generationData: data, seedOffset: seedOffset++ })
            }
            setOutputStatus(output)
          }

          if (code === "SUCCESS") {
            const endTime = Date.now()
            const timeElapsed = endTime - start
            generationTimes.push(timeElapsed)
            updateAverageGenerationTime()
            console.log(
              "Generation completed in",
              timeElapsed,
              "ms",
              generations,
              averageGenerationTime
            )
            break
          } else if (code === "PENDING") {
            if (averageGenerationTime) {
              const now = Date.now()
              let progress = ((now - start) / averageGenerationTime) * 100
              if (progress >= 100) progress = 99
              setOutputStatus({
                ok: true,
                message: "Generating...",
                progress: Math.round(progress) + "%",
              })
            } else {
              console.log("averageGenerationTime", averageGenerationTime)
              setOutputStatus({
                ok: true,
                images,
                message: "Generating...",
                progress: "Please wait...",
              })
            }
          } else {
            console.log("Generation failed", code, generations)
            setOutputStatus({ ok: false, message: generations })
            return // Stop checking
          }
          await sleep(1000)
        }
      }

      // prettier-ignore
      function _verifyGenerationOptions(data) {
        const raise = (msg) => { alert(msg); throw new Error(msg) }
        if (!data.prompt) raise("Please enter a prompt to generate an image.")
        if (data.width < 0 || data.height < 0) raise("Width and height must be positive integers.")
        if (data["batch-size"] < 1 || data["batch-count"] < 1) raise("Batch size and count must be at least 1.")
        if (data.seed < -1) raise("Seed must be a positive integer or -1 for random.")
        if (data.steps < 1) raise("Sampling steps must be at least 1.")
        if (data["control-strength"] < 0 || data["control-strength"] > 1) raise("Control strength must be between 0 and 1.")
        if (data["style-ratio"] < 0 || data["style-ratio"] > 100) raise("Style ratio must be between 0 and 100.")
        if (data["cfg-scale"] < 1 || data["cfg-scale"] > 20) raise("CFG scale must be between 1 and 20.")
        if (data["denoising-strength"] < 0 || data["denoising-strength"] > 1) raise("Denoising strength must be between 0 and 1.")
        if (data.mode === "img2img" && !data.image) raise("Please provide reference image.")
      }

      async function submitGenerationRequest() {
        const data = { model: modelStatus.model, ...currentGenerationOptions }
        _verifyGenerationOptions(data)

        let reqPayload = ""
        for (const [key, value] of Object.entries(data)) {
          reqPayload += `${key} ${value}\n`
        }
        const response = await apiCall(`/${data.mode}`, reqPayload, (ok, message) => {
          setOutputStatus({ ok, message })
        })
        const [code, [messageOrJobId]] = response
        if (code === "OK") {
          setOutputStatus({ ok: true, message: "Request submitted successfully" })
          watchGenerationJob(messageOrJobId, data)
        } else {
          setOutputStatus({ ok: false, message: [messageOrJobId, code].join("\n") })
        }
      }

      return html`<div>
        <${Header} />
        <${Content}
          models=${models}
          refreshModels=${() => {
            setModels([])
            setTimeout(() => {
              setModels([])
            }, 100)
          }}
          submitRequest=${submitGenerationRequest}
          updateActiveModel=${onModelChange}
          outputStatus=${outputStatus}
          modelStatus=${modelStatus} />
      </div>`
    }

    render(html`<${App} />`, document.body)
  </script>
</html>
)773ff122" ;