# A396347: least k such that A396346(k) = n.
#
# This uses the block structure of A396346:
# let b(m) = A396344(m), c(m) = A396345(m).
# Then for b(m) <= k < b(m + 1),
#   A396346(k) = c(m)      if k == m (mod 2),
#   A396346(k) = c(m) - k  otherwise.
#
# Therefore:
#   A396347(c(m)) = b(m),
# and for the remaining terms in block m we can fill
#   A396347(c(m) - k) = k
# directly, stepping through k with the required parity.

def next_a396344_terms(b)
  while b.length < 10
    raise 'Need 10 initial terms for A396344'
  end

  n = b.length
  b << 4 * b[n - 2] - 2 * b[n - 4] - 2 * b[n - 6] + b[n - 8]
end

def next_a396345_terms(c)
  while c.length < 10
    raise 'Need 10 initial terms for A396345'
  end

  n = c.length
  c << c[n - 1] + 4 * c[n - 2] - 4 * c[n - 3] - 2 * c[n - 4] +
       2 * c[n - 5] - 2 * c[n - 6] + 2 * c[n - 7] + c[n - 8] - c[n - 9]
end

def generate_a396347(limit)
  # Initial terms from A396344 and A396345.
  b = [0, 3, 4, 5, 10, 13, 28, 39, 86, 123]
  c = [0, 3, 6, 10, 19, 31, 58, 96, 181, 303]

  first_pos = Array.new(limit + 1)
  first_pos[0] = 0
  filled = 1
  if limit >= 1
    first_pos[1] = 2
    filled += 1
  end
  m = 1

  while filled < limit + 1
    next_a396344_terms(b) if m + 1 >= b.length
    next_a396345_terms(c) if m >= c.length

    cm = c[m]
    bm = b[m]
    bm1 = b[m + 1]

    if cm <= limit && first_pos[cm].nil?
      first_pos[cm] = bm
      filled += 1
    end

    k = bm
    k += 1 if (k - m).even?
    while k < bm1
      n = cm - k
      break if n < 0
      if n <= limit && first_pos[n].nil?
        first_pos[n] = k
        filled += 1
      end
      k += 2
    end

    m += 1
  end

  first_pos
end

n = ARGV.empty? ? 10000 : ARGV[0].to_i
ary = generate_a396347(n)
(0..n).each do |i|
  print i
  print ' '
  puts ary[i]
end
