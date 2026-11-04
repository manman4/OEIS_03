#!/usr/bin/env python3
"""Generate A398776 through antidiagonal 139 (inclusive), using Python's stdlib.

Antidiagonal r is (n,k) = (0,r+1), (1,r), ..., (r,1).
The b-file starts at index 0. No search cutoff is used for the least m.
"""

import argparse
from contextlib import contextmanager
import fcntl
import os
from pathlib import Path
import stat
import sys
import tempfile

MAX_DIAGONAL = 139
MAX_TERMS = (MAX_DIAGONAL + 1) * (MAX_DIAGONAL + 2) // 2


class PrimeTable:
    """Exact, lazily extended prime table; indices start at 1."""

    def __init__(self):
        self.values = [2]

    def get(self, index):
        while len(self.values) < index:
            candidate = 3 if self.values[-1] == 2 else self.values[-1] + 2
            while True:
                is_prime = True
                for divisor in self.values:
                    if divisor * divisor > candidate:
                        break
                    if candidate % divisor == 0:
                        is_prime = False
                        break
                if is_prime:
                    self.values.append(candidate)
                    break
                candidate += 2
        return self.values[index - 1]


def compute_values(last_diagonal):
    """Compute from scratch; saved values never affect the mathematics.

    Write S_j for the sum of the first j primes. A(n,k) is the first j-n
    with j>n and S_j == S_n (mod k). For each k, resolve a pending start
    at the first matching residue, before registering the current start.
    Thus m=0 is excluded. There is at most one pending start per residue.
    Existence follows from the Shiu argument in the remarks on problem 427.
    """
    if type(last_diagonal) is not int or not 0 <= last_diagonal <= MAX_DIAGONAL:
        raise ValueError('last diagonal must be an integer in 0..139')
    count = (last_diagonal + 1) * (last_diagonal + 2) // 2
    values = [0] * count
    primes = PrimeTable()
    for k in range(1, last_diagonal + 2):
        max_start = last_diagonal + 1 - k
        pending = {0: 0}
        remaining = max_start + 1
        residue = 0
        j = 0
        while remaining:
            j += 1
            residue = (residue + primes.get(j)) % k
            n = pending.pop(residue, None)
            if n is not None:
                diagonal = n + k - 1
                index = diagonal * (diagonal + 1) // 2 + n
                values[index] = j - n
                remaining -= 1
            if j <= max_start:
                pending[residue] = j
    return values


def _regular_single_link(fd, label):
    info = os.fstat(fd)
    if not stat.S_ISREG(info.st_mode) or info.st_nlink != 1:
        raise ValueError(f'{label} must be a regular file with one hard link')
    return info


def _stamp(info):
    return (info.st_dev, info.st_ino, info.st_size,
            info.st_mtime_ns, info.st_ctime_ns)


def _current_stamp(path):
    try:
        info = path.lstat()
    except FileNotFoundError:
        return None
    if not stat.S_ISREG(info.st_mode) or info.st_nlink != 1:
        raise ValueError('output must be a regular file with one hard link')
    return _stamp(info)


@contextmanager
def output_lock(path):
    """Keep the stable lock file; removing it can invalidate mutual exclusion."""
    lock_path = path.with_name(path.name + '.lock')
    fd = os.open(lock_path, os.O_CREAT | os.O_RDWR | os.O_NOFOLLOW, 0o600)
    try:
        _regular_single_link(fd, 'lock')
        try:
            fcntl.flock(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError as exc:
            raise RuntimeError('another process is using this output') from exc
        yield
    finally:
        os.close(fd)


def read_existing(path):
    try:
        fd = os.open(path, os.O_RDONLY | os.O_NOFOLLOW | os.O_NONBLOCK)
    except FileNotFoundError:
        return [], None
    with os.fdopen(fd, 'r', encoding='ascii') as source:
        info = _regular_single_link(source.fileno(), 'output')
        if info.st_size > 1_000_000:
            raise ValueError('existing b-file is unexpectedly large')
        values = []
        for line in source:
            fields = line.split()
            if not fields or line.lstrip().startswith('#'):
                continue
            if len(fields) != 2 or not all(x.isascii() and x.isdigit() for x in fields):
                raise ValueError('invalid b-file row')
            index, value = map(int, fields)
            if index != len(values) or value < 1 or len(values) >= MAX_TERMS:
                raise ValueError('invalid b-file index, value, or length')
            values.append(value)
        complete_counts = {r * (r + 1) // 2 for r in range(MAX_DIAGONAL + 2)}
        if len(values) not in complete_counts:
            raise ValueError('existing b-file ends inside an antidiagonal')
        if _stamp(os.fstat(source.fileno())) != _stamp(info):
            raise RuntimeError('output changed while being read')
    return values, _stamp(info)


def atomic_write(path, values, previous_stamp):
    fd, name = tempfile.mkstemp(prefix='.' + path.name + '.', suffix='.tmp',
                                dir=path.parent)
    temporary = Path(name)
    try:
        with os.fdopen(fd, 'w', encoding='ascii', newline='\n') as target:
            for index, value in enumerate(values):
                target.write(f'{index} {value}\n')
            target.flush()
            os.fsync(target.fileno())
        if _current_stamp(path) != previous_stamp:
            raise RuntimeError('output changed; refusing to overwrite it')
        if previous_stamp is None:
            # Publish without overwriting a file created by another program.
            os.link(temporary, path)
            temporary.unlink()
        else:
            os.replace(temporary, path)
        directory_fd = os.open(path.parent, os.O_RDONLY | os.O_DIRECTORY)
        try:
            os.fsync(directory_fd)
        except OSError as exc:
            raise OSError('output published, but directory synchronization failed') from exc
        finally:
            os.close(directory_fd)
    finally:
        temporary.unlink(missing_ok=True)


def save_bfile(path, last_diagonal):
    """Lock before reading. Compare the common prefix before any replacement."""
    path = Path(path).expanduser().absolute()
    # Canonicalize the parent, but do not follow a final symlink.
    path = path.parent.resolve(strict=True) / path.name
    if type(last_diagonal) is not int or not 0 <= last_diagonal <= MAX_DIAGONAL:
        raise ValueError('last diagonal must be an integer in 0..139')
    with output_lock(path):
        old_values, old_stamp = read_existing(path)
        values = compute_values(last_diagonal)
        overlap = min(len(values), len(old_values))
        if values[:overlap] != old_values[:overlap]:
            raise ValueError('existing values disagree; output has not been changed')
        if old_values and len(old_values) >= len(values):
            return len(old_values), False
        atomic_write(path, values, old_stamp)
        return len(values), True


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('last_diagonal', nargs='?', type=int, default=139,
                        help='last antidiagonal, inclusive (0..139; default: 139)')
    parser.add_argument('--output', type=Path,
                        default=Path(__file__).with_name('b398776.txt'),
                        help='b-file path (default: beside this script)')
    args = parser.parse_args()
    if not 0 <= args.last_diagonal <= MAX_DIAGONAL:
        parser.error('last_diagonal must be in 0..139')
    try:
        count, changed = save_bfile(args.output, args.last_diagonal)
    except KeyboardInterrupt:
        print('Interrupted. Run again to recompute; no partial b-file is published.',
              file=sys.stderr)
        return 130
    except (OSError, ValueError, RuntimeError) as exc:
        print(f'Error: {exc}', file=sys.stderr)
        return 1
    action = 'Saved' if changed else 'Kept existing'
    print(f'{action} {count} terms (indices 0..{count - 1}).', file=sys.stderr)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
