def a309807(n)
  return 1 if n == 0
  a = (1..n).map{|i| [i]}
  len = 1
  while len < n
    b = Array(Array(Int32)).new
    a.each{|c|
      (1..n).each{|num|
        if !c.includes?(num)
          i = c.clone + [num]
          if (i[-2] * (len + 1)) > (i[-1] * len)
            b << i
          end
        end
      }
    }
    a = b
    len += 1
  end
  a.size
end

p (0..50).map{|i| p a309807(i)}