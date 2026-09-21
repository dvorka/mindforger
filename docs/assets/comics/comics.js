(function () {
    'use strict';

    var root = document.getElementById('cx');
    if (!root) {
        return;
    }

    /* ================================================ pixel faces - the same you as in the diagram */

    var FACE_PALETTE = {
        h: '#3b2414', H: '#6b4423', s: '#e0a070', S: '#b8784a', e: '#c98a58',
        w: '#f5f5f5', b: '#3b2414', m: '#4a1010', t: '#fafafa'
    };

    var FACE_BASE = [
        '....hhhhhhhh....',
        '..hhhHhhhhHhhh..',
        '.hhHhhhhhhhhHhh.',
        '.hhsshhsshhsshh.',
        '.hssssssssssssh.',
        '.sbbbbssssbbbbs.',
        'esswwwsssswwwsse',
        'esswwwsSSswwwsse',
        '.sswwwsSSswwwss.',
        '.sSsssSSSSsssSs.',
        '.ssssssssssssss.',
        '..ssssssssssss..',
        '..ssssssssssss..',
        '...SssssssssS...',
        '....SSssssSS....',
        '.....SSSSSS.....'
    ];

    // expression = rows overriding the base face
    var FACE_MOODS = {
        calm: { 11: '..ssssmmmmssss..' },
        grin: { 10: '.ssmsssssssssms.', 11: '..smttttttttms..', 12: '..ssmmmmmmmmss..' },
        puzzled: { 4: '.hsbbbssssssssh.', 5: '.sssssssssbbbbs.', 11: '..sssssmmsssss..', 12: '..sssssmmsssss..' },
        wow: { 4: '.hbbbbssssbbbbh.', 5: '.ssssssssssssss.', 11: '..sssssmmsssss..', 12: '..sssssmmsssss..' }
    };

    // pupil offsets from eye center
    var LOOK = { c: [0, 0], l: [-1, 0], r: [1, 0], d: [0, 1], u: [0, -1], ul: [-1, -1], ur: [1, -1] };

    // horizontal runs of the same color are merged to keep the svg small and seamless
    function pixelRects(rows) {
        var out = '';
        rows.forEach(function (row, y) {
            var x = 0;
            while (x < row.length) {
                var ch = row[x];
                var len = 1;
                while (row[x + len] === ch) {
                    len++;
                }
                if (FACE_PALETTE[ch]) {
                    out += '<rect x="' + x + '" y="' + y + '" width="' + len + '" height="1" fill="' + FACE_PALETTE[ch] + '"/>';
                }
                x += len;
            }
        });
        return out;
    }

    function drawFace(svg, look, mood) {
        var rows = FACE_BASE.slice();
        var overrides = FACE_MOODS[mood] || FACE_MOODS.calm;
        Object.keys(overrides).forEach(function (y) {
            rows[y] = overrides[y];
        });
        var o = LOOK[look] || LOOK.c;
        var pupils = [4, 11].map(function (x) {
            return '<rect x="' + (x + o[0]) + '" y="' + (7 + o[1]) + '" width="1" height="1" fill="#111"/>';
        }).join('');
        svg.innerHTML = pixelRects(rows) + pupils;
    }

    Array.prototype.forEach.call(root.querySelectorAll('.cx-you'), function (you) {
        drawFace(you.querySelector('svg'), you.dataset.look, you.dataset.mood);
    });

    /* ================================================ pager */

    var pages = root.querySelectorAll('.cx-page');
    var prev = document.getElementById('cx-prev');
    var next = document.getElementById('cx-next');
    var NEXT_LABEL = next.textContent;
    var NEXT_LAST_LABEL = 'Back to MindForger docs';
    var dots = document.getElementById('cx-dots');
    var count = document.getElementById('cx-count');
    var current = -1;

    Array.prototype.forEach.call(pages, function (page, i) {
        var dot = document.createElement('button');
        dot.type = 'button';
        dot.setAttribute('role', 'tab');
        dot.setAttribute('aria-label', 'Page ' + (i + 1));
        dot.addEventListener('click', function () {
            show(i);
        });
        dots.appendChild(dot);

        // click on the art turns the page - left third back, the rest forward
        var art = page.querySelector('.cx-art');
        art.addEventListener('click', function (e) {
            if (e.target.closest('a')) {
                return;
            }
            var box = art.getBoundingClientRect();
            show(e.clientX - box.left < box.width / 3 ? current - 1 : current + 1);
        });
    });

    function show(i, keepHash) {
        if (i < 0 || i >= pages.length || i === current) {
            return;
        }
        if (current >= 0) {
            pages[current].classList.remove('cx-on');
            pages[current].setAttribute('aria-hidden', 'true');
        }
        current = i;
        pages[i].classList.add('cx-on');
        pages[i].removeAttribute('aria-hidden');
        Array.prototype.forEach.call(dots.children, function (dot, d) {
            dot.setAttribute('aria-current', d === i ? 'true' : 'false');
        });
        prev.disabled = i === 0;
        // the last page has no next page - the button leads back to the documentation
        var last = i === pages.length - 1;
        next.textContent = last ? NEXT_LAST_LABEL : NEXT_LABEL;
        next.setAttribute('aria-label', last ? NEXT_LAST_LABEL : 'Next page');
        count.textContent = 'page ' + (i + 1) + ' / ' + pages.length;
        if (!keepHash && history.replaceState) {
            history.replaceState(null, '', '#cx-' + (i + 1));
        }
    }

    prev.addEventListener('click', function () {
        show(current - 1);
    });
    next.addEventListener('click', function () {
        if (current === pages.length - 1) {
            window.location.href = root.getAttribute('data-home') || '../../index.html';
        } else {
            show(current + 1);
        }
    });

    // arrow keys turn pages while the comic is on screen
    document.addEventListener('keydown', function (e) {
        if (e.altKey || e.ctrlKey || e.metaKey || /input|textarea|select/i.test(e.target.tagName)) {
            return;
        }
        var box = root.getBoundingClientRect();
        if (box.bottom < 0 || box.top > window.innerHeight) {
            return;
        }
        if (e.key === 'ArrowRight') {
            show(current + 1);
        } else if (e.key === 'ArrowLeft') {
            show(current - 1);
        }
    });

    // swipe on touch screens
    var touchX = null;
    root.addEventListener('touchstart', function (e) {
        touchX = e.touches[0].clientX;
    }, { passive: true });
    root.addEventListener('touchend', function (e) {
        if (touchX === null) {
            return;
        }
        var dx = e.changedTouches[0].clientX - touchX;
        touchX = null;
        if (Math.abs(dx) > 40) {
            show(dx < 0 ? current + 1 : current - 1);
        }
    }, { passive: true });

    // deep link - #cx-3 opens the third page
    var m = /^#cx-(\d+)$/.exec(location.hash);
    show(m ? Math.min(Math.max(parseInt(m[1], 10), 1), pages.length) - 1 : 0, true);
    if (m) {
        // wait for images above the comic so that the scroll position holds
        if (document.readyState === 'complete') {
            root.scrollIntoView();
        } else {
            window.addEventListener('load', function () {
                root.scrollIntoView();
            });
        }
    }
})();
