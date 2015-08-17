

# pebble pouch -- created at Hack the Planet 2015

![Alt text](http://challengepost-s3-challengepost.netdna-ssl.com/photos/production/software_photos/000/281/677/datas/gallery.jpg "pebble pouch img")

##Inspiration
We're college students. We balance our social lives with our wallet--but sometimes it can be difficult. When we need to decide in a few seconds whether or not we'll order that late night boba or last-minute trip off campus, we don't have time to log on with our browsers to access our bank account balance. That's where "pebble pouch" comes in.

##How it works
Pebble pouch can help people of all ages meet their budgeting goals. All you need is a smartphone (Android or iOS!) and a Pebble! Set a timeline for your budget, an amount, and fill in your account info from Capital One. Pebble pouch is a Pebble watchapp that seamlessly integrates with Android and iOS. It's best used on the Pebble Time, with a beautiful color-alerting background. It clearly presents how much money you have left to spend in your budget, along with how many days/hours you have to spend it in. At the bottom, there's a progress bar that will show you how close you are to spending all your budget. And as you get closer to the top of your budget, the background changes from green to yellow to red.

Need to know how much is in all of your account? That's displayed, too!

Want to know where all your money has gone? See your previous bills by clicking the middle button in the app.

##Challenges
We ran into some challenges with memory issues in C while developing for Pebble. But after some review, we made it through. It was also difficult to write the "Settings" page, which required a backend server set up to communicate with the Javascript in the Pebble app.

##Accomplishments
We are especially proud of how easy the app is to use. We worked hard on arranging the text layers and choosing intuitive colors. We're running a local server using Node.js so that users can enter custom settings. We make that server public using NGrok so that anyone with a Pebble has access to it.

![Alt text](http://challengepost-s3-challengepost.netdna-ssl.com/photos/production/software_photos/000/281/680/datas/gallery.jpg "pebble pouch img")

##What's next for pebble pouch
In the future, we would want to be able to add pins to Pebble Time's Timeline feature that would mark where you spend your money. It would also be great to further examine bills (maybe using GPS coordinates) in the app.
