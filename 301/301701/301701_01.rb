def A(n)
  ary = [1]
  s_ary = Array.new(n + 1, 0)
  (1..500).each{|i|
    a = ary.map{|i| -i} + [0] * i
    (0..(i - 1) * i / 2).each{|j| a[i + j] += ary[j]}
    ary = a
    ary.each{|j|
      if j >= 0 && j <= n
        k = s_ary[j]
        s_ary[j] = i if k == 0
      end
    }
  }
  s_ary
end

n = 10000
ary = A(n)
(0..n).each{|i|
  j = ary[i]
  break if j == 0
  print i
  print ' '
  puts j
}
