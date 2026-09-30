#!/usr/bin/env python3
"""xeno_drive.py -- HARNESS ONLY: drive xeno-port on a PRIVATE Xvfb display.

Used to record the real story route (XENO_ROUTE_LOG) and to take dev-menu
screenshots.  Never touches any other display: it starts its own Xvfb on a free
display number, points DISPLAY at it explicitly and unsets WAYLAND_DISPLAY.

  xeno_drive.py OUTDIR SCRIPT
    wait <timeout_s> <needle>      wait for a NEW log line containing needle
    waitmap <timeout_s> <map>      wait for a POSDIAG line with that map
    send <console command>         stdin cheat console (XENO_CHEATS_CONSOLE)
    sleep <s>
    key <X keysym> [hold_s]        XTest tap
    keydown / keyup <keysym>
    grab <name>                    X screenshot -> OUTDIR/<name>.ppm
    loadqs                         send `load` until the checkpoint is accepted
    sh <shell command>
  a SCRIPT of the form fifo:/path/to/fifo reads commands from that FIFO until `quit`
  env: BIN, QS (quicksave path), EXTRA ("K=V K=V"), SECS (hard timeout),
       XENO_ROOT (working dir), XENO_DATA_DIR (disc files), NICE
"""
import os, sys, time, subprocess, threading

sys.path.insert(0, os.path.expanduser('~/release-staging/xeno-work/r6-nogamedata/pylib'))
from Xlib import X, XK, display as xdisplay
from Xlib.ext import xtest

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.environ.get('XENO_ROOT', os.path.abspath(os.path.join(HERE, '..', '..')))
BIN = os.environ.get('BIN', ROOT + '/pc_port/build_native/xeno-port')
DISC = os.environ.get('XENO_DATA_DIR', ROOT + '/disc')


class Session:
    def __init__(self, out, script):
        self.out, self.script = out, script
        os.makedirs(out, exist_ok=True)
        n = next(i for i in range(160, 260)
                 if not os.path.exists('/tmp/.X11-unix/X%d' % i) and not os.path.exists('/tmp/.X%d-lock' % i))
        self.dpy = ':%d' % n
        self.xv = subprocess.Popen(['Xvfb', self.dpy, '-screen', '0', '1280x1024x24', '-nolisten', 'tcp'],
                                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        time.sleep(2)
        env = dict(os.environ)
        env.pop('WAYLAND_DISPLAY', None)
        env.pop('XAUTHORITY', None)
        xdg = os.path.join(out, 'xdg')
        for d in ('config', 'data', 'cache', 'state', 'run'):
            os.makedirs(os.path.join(xdg, d), exist_ok=True)
        os.chmod(os.path.join(xdg, 'run'), 0o700)
        env.update(DISPLAY=self.dpy, SDL_VIDEODRIVER='x11', SDL_AUDIODRIVER='dummy', ALSOFT_DRIVERS='null',
                   XENO_DATA_DIR=DISC, XENO_DISC=DISC + '/disc1.bin', XENO_VIDEO_FULLSCREEN='off',
                   XDG_CONFIG_HOME=xdg + '/config', XDG_DATA_HOME=xdg + '/data', XDG_CACHE_HOME=xdg + '/cache',
                   XDG_STATE_HOME=xdg + '/state', XDG_RUNTIME_DIR=xdg + '/run',
                   XENO_MEMCARD_DIR=xdg + '/data/memcards', XENO_CHEATS_CONSOLE='1',
                   XENO_DEV_MENU='1', XENO_DEV_MENU_UNVERIFIED='1',
                   XENO_QUICKSAVE_PATH=os.environ.get('QS', os.path.join(out, 'qs.xgqs')))
        for kv in os.environ.get('EXTRA', '').split():
            k, v = kv.split('=', 1)
            env[k] = v
        self.t0 = time.time()
        self.log = open(os.path.join(out, 'run.log'), 'w', buffering=1, errors='replace')
        self.proc = subprocess.Popen(['nice', '-n', os.environ.get('NICE', '10'), BIN], cwd=ROOT, env=env,
                                     stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        self.lines, self.lock = [], threading.Lock()
        threading.Thread(target=self._reader, daemon=True).start()

    def _reader(self):
        for raw in self.proc.stdout:
            l = raw.decode('latin1', 'replace').rstrip('\n')
            with self.lock:
                self.lines.append(l)
            self.log.write('%8.2f %s\n' % (time.time() - self.t0, l))

    def mark(self, t):
        self.log.write('%8.2f [driver] %s\n' % (time.time() - self.t0, t))
        print('[+%.1f] %s' % (time.time() - self.t0, t), flush=True)

    def send(self, c):
        self.mark('stdin: ' + c)
        self.proc.stdin.write((c + '\n').encode())
        self.proc.stdin.flush()

    def wait_for(self, needle, timeout, start=0):
        end = time.time() + timeout
        while time.time() < end:
            with self.lock:
                for i in range(start, len(self.lines)):
                    if needle in self.lines[i]:
                        return i
            if self.proc.poll() is not None:
                return None
            time.sleep(0.2)
        return None

    def last(self, needle):
        with self.lock:
            for l in reversed(self.lines):
                if needle in l:
                    return l
        return None

    def _xd(self):
        d = xdisplay.Display(self.dpy)
        root = d.screen().root
        tgt = None
        for w in root.query_tree().children:
            try:
                if w.get_attributes().map_state == X.IsViewable and w.get_geometry().width > 100:
                    tgt = w
            except Exception:
                pass
        return d, root, tgt

    def key(self, k, down=True, up=True, hold=0.12):
        d, root, tgt = self._xd()
        if tgt is not None:
            tgt.set_input_focus(X.RevertToParent, X.CurrentTime)
            d.sync()
        code = d.keysym_to_keycode(XK.string_to_keysym(k))
        if down:
            xtest.fake_input(d, X.KeyPress, code)
            d.sync()
        if down and up:
            time.sleep(hold)
        if up:
            xtest.fake_input(d, X.KeyRelease, code)
            d.sync()
        d.close()

    def grab(self, name):
        d, root, tgt = self._xd()
        x0, y0, w, h = 0, 0, 640, 480
        if tgt is not None:
            g = tgt.get_geometry()
            x0, y0, w, h = g.x, g.y, g.width, g.height
        img = root.get_image(x0, y0, w, h, X.ZPixmap, 0xFFFFFFFF)
        data = img.data if isinstance(img.data, bytes) else img.data.encode('latin1')
        rgb = bytearray(w * h * 3)
        rgb[0::3], rgb[1::3], rgb[2::3] = data[2::4], data[1::4], data[0::4]
        with open(os.path.join(self.out, name + '.ppm'), 'wb') as f:
            f.write(b'P6 %d %d 255\n' % (w, h) + bytes(rgb))
        d.close()
        self.mark('grab ' + name)

    def loadqs(self):
        for _ in range(60):
            with self.lock:
                s = len(self.lines)
            self.send('load')
            if self.wait_for('[xeno-port][quick] loading', 5, s) is not None:
                return True
            time.sleep(3)
        return False

    def close(self):
        if self.proc.poll() is None:
            self.proc.terminate()
            try:
                self.proc.wait(15)
            except subprocess.TimeoutExpired:
                self.proc.kill()
                self.proc.wait()
        self.xv.terminate()
        self.xv.wait()
        self.log.close()


def serve_lines(fifo):
    """Yield command lines from a FIFO until `quit` (interactive route driving)."""
    while True:
        with open(fifo) as f:
            for ln in f:
                if ln.strip() == 'quit':
                    return
                yield ln


def run_script(s, script):
    deadline = s.t0 + float(os.environ.get('SECS', '900'))
    rc = 0
    lines = serve_lines(script[5:]) if script.startswith('fifo:') else open(script)
    for ln in lines:
        ln = ln.strip()
        if not ln or ln.startswith('#'):
            continue
        if time.time() > deadline or s.proc.poll() is not None:
            s.mark('ABORT deadline/exit')
            return 1
        if ln.startswith('#'):
            continue
        op, _, arg = ln.partition(' ')
        if op == 'wait':
            t, _, needle = arg.partition(' ')
            with s.lock:
                start = len(s.lines)
            if s.wait_for(needle, float(t), start) is None:
                s.mark('WAIT FAIL ' + needle)
                return 1
            s.mark('got ' + needle)
        elif op == 'waitmap':
            t, _, m = arg.partition(' ')
            if s.wait_for('POSDIAG map=%s ' % m, float(t), 0) is None:
                s.mark('WAITMAP FAIL ' + m)
                return 1
        elif op == 'send':
            s.send(arg)
        elif op == 'sleep':
            time.sleep(float(arg))
        elif op == 'key':
            a = arg.split()
            s.key(a[0], hold=float(a[1]) if len(a) > 1 else 0.12)
        elif op == 'keydown':
            s.key(arg, up=False)
        elif op == 'keyup':
            s.key(arg, down=False)
        elif op == 'grab':
            s.grab(arg)
        elif op == 'sh':
            subprocess.run(arg, shell=True)
        elif op == 'loadqs':
            if not s.loadqs():
                s.mark('LOAD FAIL')
                return 1
    return rc


if __name__ == '__main__':
    sess = Session(sys.argv[1], sys.argv[2])
    try:
        rc = run_script(sess, sys.argv[2])
    finally:
        sess.close()
    sys.exit(rc)
