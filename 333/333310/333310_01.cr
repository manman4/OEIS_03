def a(k, m, n)
  a = (m..m + n - 1).map{|i| [i]}
  len = 1
  while len < n
    b = Array(Array(Int32)).new
    a.each{|c|
      (m..m + n - 1).each{|num|
        if !c.includes?(num)
          i = c.clone + [num]
          if (i[-2] * (len + k + 1)) > (i[-1] * (len + k))
            b << i
          end
        end
      }
    }
    a = b
    len += 1
  end
  ary = Array.new(n, 0)
  a.each{|c| ary[c[0] - m] += 1}
  ary
end

n = 18
ary = Array(Int32).new
(1..n).each{|i|
  ary += a(0, 1, i)
}

(1..ary.size).each{|i|
  print i
  print " "
  puts ary[i - 1]
}
