def ncr(n, r)
  return 1 if r == 0
  (n - r + 1..n).inject(:*) / (1..r).inject(:*)
end

# T(n,k) = (n*Sum_{j=1..floor(n/k)} binomial(j,n-k*j)/j) mod n.  (n >= 2, 1 <= k <= floor(n/2)).
def T(n, k)
  ((1..n / k).inject(0){|sum, j| sum + ncr(j, n - k * j) / j.to_r} * n).to_i % n
end

n = 200
cnt = 2
# 2からn行
(2..n).each{|i|
  (1..i / 2).each{|j|
    print cnt
    print " "
    puts T(i, j)
    cnt += 1
  }
}
