def a(k,n)
  return 1 if n == 0
  a = (1..n).map{|i| [i]}
  len = 1
  while len < n
    b = Array(Array(Int32)).new
    a.each{|c|
      (1..n).each{|num|
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
  a.size
end

def b(k,n)
  return 1 if n == 0
  a = (1..n).map{|i| [i]}
  len = 1
  while len < n
    b = Array(Array(Int32)).new
    a.each{|c|
      (1..n).each{|num|
        if !c.includes?(num)
          i = c.clone + [num]
          if (i[-2] * (len + k + 1)) >= (i[-1] * (len + k))
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


# (0..0).each{|j|
#   p a = (1..15).map{|i| a(j,i)}
#   p b = (0..14).map{|i| b(j,i)}
#  p [j, a == b]
# }
# (1..6).each{|j|
#   p a = (1..15 + j).map{|i| a(j,i)}
#   p b = (0..14 + j).map{|i| b(j,i)}
#  p [j, a == b]
# }
# (7..10).each{|j|
#   p a = (1..13 + j).map{|i| a(j,i)}
#   p b = (0..12 + j).map{|i| b(j,i)}
#  p [j, a == b]
# }
# (11..14).each{|j|
#   p a = (1..20).map{|i| a(j,i)}
#   p b = (0..19).map{|i| b(j,i)}
#  p [j, a == b]
# }
# (15..30).each{|j|
#  p [j, (1..20).map{|i| a(j,i)} == (0..19).map{|i| b(j,i)}]
# }

# p (0..15).map{|i| p a(i,2*i+4)}

# p a = (0..18).map{|i| p a(0,i)}
# p  [1] + (1..18).map{|i| a[i] - a[i-1]}

cnt = 0
(0..18).each{|i|
  (0..i).each{|j|
    print cnt
    print ' '
    puts a(j, i)
    cnt += 1
  }
}