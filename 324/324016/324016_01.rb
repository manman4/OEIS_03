def A(n)
  d = 0
  a = (1..n - 1).select{|i| i % 2 == 0}.map{|i| [i]}
  while d < n - 2 && a.size > 0
    d += 1
    b = []
    a.each{|i|
      (1..n - 1).each{|j|
        if !i.include?(j)
          m = i.clone + [j]
          b << m if (0..d).inject(0){|s, k| s + m[k] * 10 ** (d - k)} % (d + 2) == 0
        end
      }
    }
    a = b
  end
  a.size
end

(2..31).each{|i|
  print i
  print ' '
  puts A(i)
}
