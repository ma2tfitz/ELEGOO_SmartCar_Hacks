filename = "slowturn_2.log"
#filename = "slowturn_6.log" # keep
#filename = "slowturn_7.log"
#filename = "crash.log" # keep
a = read.delim(filename)

x = a$time
if (is.null(x)) {
    x = (1:nrow(a))/20
} else {
    x = (a$time - min(a$time))/1000
}
plot(x, a$tz - min(a$tz), type="b", main=filename, sub=median(a$right/a$left))
lines(x, a$tx - min(a$tx), col="green", type="b")
lines(x, a$ty - min(a$ty), col="blue", type="b")

if ("tag" %in% names(a)) {
    sel = a$tag != ""
    abline(v=x[sel])
}

lines(x, a$left*0.735, type="b", col="green")
lines(x, a$right, type="b", col="blue")
