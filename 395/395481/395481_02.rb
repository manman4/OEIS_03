# PARI/GP版
# T(n, k) = if(n==0, k==0, (k+1)*T(n-1, k)+(n-k)*T(n-1, k-1)+(-1)^n*(k==n-1));
# を Ruby に移植

$memo = {}

def t(n, k)
  key = [n, k]
  return $memo[key] if $memo.key?(key)

  val = if n == 0
    k == 0 ? 1 : 0
  else
    term1 = (k + 1) * t(n - 1, k)
    term2 = (n - k) * t(n - 1, k - 1)
    term3 = ((-1) ** n) * (k == n - 1 ? 1 : 0)
    term1 + term2 + term3
  end

  $memo[key] = val
end

# for(n=0, 12, for(k=0, n, print1(T(n,k),", ")));
(0..12).each{|n|
  (0..n).each{|k|
    print "#{t(n, k)}, "
  }
}
puts

# vector(13, n, T(n-1,0..3))
(0..3).each{|k|
  arr = (1..13).map{|n| t(n - 1, k)}
  p arr
}
