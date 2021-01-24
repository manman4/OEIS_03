def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(k, n)
  ary = [1, 2 * k]
  (2..n).each{|i|
    ary << (2 * (2 * (k -1) * i * i - k + 2) * ary[-1] - (k - 3) ** 2 * i * (2 * i + 1) * ary[-2]) / (i * (2 * i - 1))
  }
  ary
end

(0..6).each{|i|
  p [i, A(i, 20)]
}
