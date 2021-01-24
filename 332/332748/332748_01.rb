def search(a, num, d, n)
  if num == 0
    @count += 1
  else
    (3 * n - 2).times{|i|
      if a[i] == 0
        (i + d + 1..3 * n - 2 - d).each{|j|
          if a[j] == 0 && (2 * j - i < 3 * n)
            if a[2 * j - i] == 0
              a[i], a[j], a[2 * j - i] = num, num, num
              search(a, num - 1, j - i - 1, n)
              a[i], a[j], a[2 * j - i] = 0, 0, 0
            end
          end
        }
      end
    }
  end
end

def A(n)
  a = [0] * n * 3
  @count = 0
  search(a, n, 0, n)
  @count
end

puts "A332748"
p (0..10).map{|i| p A(i)}

# def search(a, num, d, k, n)
#   if num == 0
#     @count += 1
#   else
#     (3 * n - 2).times{|i|
#       if a[i] == 0
#         (i + d + 1..3 * n - 1).each{|j|
#           if a[j] == 0 && (2 * j - i < 3 * n)
#             if a[2 * j - i] == 0
#               a[i], a[j], a[2 * j - i] = num, num, num
#               search(a, num - 1, j - i - k, k, n)
#               a[i], a[j], a[2 * j - i] = 0, 0, 0
#             end
#           end
#         }
#       end
#     }
#   end
# end

# def A(k, n)
#   a = [0] * n * 3
#   @count = 0
#   search(a, n, 0, k, n)
#   @count
# end

# puts "A060963"
# p (0..9).map{|i| p A(0, i)}
# puts "A322178"
# p (0..6).map{|i| p A(1, i)}