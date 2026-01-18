FROM maven:3.9.9-eclipse-temurin-21 AS build
COPY ./app /app
RUN cd /app && \
    mvn -v && \
    mvn -B -DskipTests package

FROM eclipse-temurin:21-jre
COPY --from=build /app/target/*.jar /opt/app.jar
CMD ["java","-jar","/opt/app.jar"]

