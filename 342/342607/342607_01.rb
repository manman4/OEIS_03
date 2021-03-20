require 'prime'

def phi(n)
  (1..n).count{|i| i.gcd(n) == 1}
end

def A(n)
  # 計算途中に分数が出るが、最終的に整数値
  (1..n).inject(0){|s, i| s + phi(n / (i.gcd(n))) ** (n - n / (i.gcd(n)) - 1)}.to_i
  
  # s = 0
  # (1..n).each{|i|
  #   p s
  #   s += phi(n / (i.gcd(n))) ** (n - n / (i.gcd(n)) - 1)
  # }
  # s
end

n = 10
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  # FORMULA確認
  if i.prime?
    k = 2
    break if j != k
  end
  print i
  print ' '
  puts j
}
