def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# n >= 0, m = 1 or 2
def chebyshev(n, m, k)
  if m == 2
    return (0..n).inject(0){|s, i| s + (2 * k - 2) ** i * ncr(n + 1 + i, 2 * i + 1)}
  elsif m == 1
    if n == 0
      return 1
    else
      return (n * (0..n).inject(0){|s, i| s + (2 * k - 2) ** i * ncr(n + i, 2 * i) / (n + i.to_r)}).to_i
    end
  end
end

def A341576(n)
  (0..n).inject(0){|s, i| s + chebyshev(i, 2, (n - i) / 2r).to_i}
end

n = 26
(0..n).each{|i|
  j = A341576(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
