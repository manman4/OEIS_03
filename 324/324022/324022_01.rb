def A(n)
  d = 0
  a = (1..n - 1).map{|i| [i]}
  ary = (1..n - 1).to_a
  while d < n - 2
    d += 1
    b = []
    a.each{|i|
      (1..n - 1).each{|j|
        m = i.clone + [j]
        b << m if (0..d).inject(0){|s, k| s + m[k] * n ** (d - k)} % (d + 1) == 0
      }
    }
    a = b
    ary += a.map{|i| i.join.to_i}
  end
  ary
end

ary = A(10)
(1..ary.size).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
