def sigma_2(n)
  cnt = 0
  (1..n).each{|i|
    cnt += i * i if n % i == 0
  }
  cnt
end

def A320649(n)
  s_ary = (0..n).map{|i| sigma_2(i)}
  ary = [1]
  (1..n).each{|i|
    ary << (1..i).inject(0){|s, j| s + s_ary[j] * ary[-j]}
  }
  ary
end

n = 2000
ary = A320649(n)
(0..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
  