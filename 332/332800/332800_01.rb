def A(n)
  a = (1..n).map{|i| [i]}
  len = 1
  while len < n
    b = []
    a.each{|c|
      (1..n).each{|num|
        if !c.include?(num)
          i = c.clone
          j = i + [num]
          if len > 1
            if (j[-3] % j[-2]) <= (j[-2] % j[-1])
              b << j
            end
          else
            b << j
          end
        end
      }
    }
    a = b
    len += 1
  end
  a
end

p (1..35).map{|i| p [i, A(i).size]}