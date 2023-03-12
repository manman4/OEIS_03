def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

def A(n, k)
  (0..n / 2).inject(0){|s, i| s + k ** (n - i) * ncr(n, 2 * i)}
end

n = 139
cnt = 0
(0..n).each{|i|
  (0..i).each{|j|
    k = A(j, i - j)
    break if k.to_s.size > 1000
    print cnt
    print ' '
    puts k
    cnt +=1
  }
}
