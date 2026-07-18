# a(n) = 3^(n-1) - Sum_{k=1..floor(n/4)} 3^(n-4*k) * a(k).
def A(n)
  ary = [0]
  (1..n).each{|i|
    s = 3 ** (i - 1)
    (1..(i / 4)).each{|k|
      s -= 3 ** (i - 4 * k) * ary[k]
    }
    ary << s
  }
  ary
end

n = 50
p ary = A(n)
(1..n).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
