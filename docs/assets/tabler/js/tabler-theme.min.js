/*!
* Tabler v1.5.1 (https://tabler.io)
* Copyright 2018-2026 The Tabler Authors
* Copyright 2018-2026 codecalm.net Paweł Kuna
* Licensed under MIT (https://github.com/tabler/tabler/blob/master/LICENSE)
*/
!function(e){"function"==typeof define&&define.amd?define([],e):e()}(function(){var e={theme:"auto","theme-base":"gray","theme-font":"sans-serif","theme-primary":"blue","theme-radius":"1",layout:"default",navbar:"default","navbar-position":"horizontal","navbar-theme":"default",sidebar:"default"},t=new Proxy(new URLSearchParams(window.location.search),{get:(e,t)=>e.get(t)}),a=window.matchMedia("(prefers-color-scheme: dark)");for(const o in e){const r=t[o];let m;if(r)localStorage.setItem("tabler-"+o,r),m=r;else{var n;const t=localStorage.getItem("tabler-"+o),a=document.documentElement.getAttribute("data-bs-"+o);m=null!==(n=null!=t?t:a)&&void 0!==n?n:e[o]}"theme"===o&&"auto"===m&&(m=a.matches?"dark":"light"),m!==e[o]?document.documentElement.setAttribute("data-bs-"+o,m):document.documentElement.removeAttribute("data-bs-"+o)}a.addEventListener("change",e=>{var t;"auto"===(null!==(t=localStorage.getItem("tabler-theme"))&&void 0!==t?t:"auto")&&(e.matches?document.documentElement.setAttribute("data-bs-theme","dark"):document.documentElement.removeAttribute("data-bs-theme"))})});
//# sourceMappingURL=tabler-theme.min.js.map