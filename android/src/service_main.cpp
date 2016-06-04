#include <android/log.h>
#include <jni.h>

#include <QHostAddress>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QThread>

#include "service.h"
#include "service_main.h"

static QAndroidJniObject toyVpnServiceJava;

JNIEXPORT void JNICALL Java_com_netvirt_netvirt_ToyVpnServiceQt_main(JNIEnv *env,
                                                                     jobject thisObj,
                                                                     jobject toyVpnServiceJava_,
                                                                     jstring server_host_,
                                                                     jint server_port_,
                                                                     jstring secret_) {
    Q_UNUSED(env);
    Q_UNUSED(thisObj);
    __android_log_write(ANDROID_LOG_INFO, "ToyVpnService", "beginning of main()");

    toyVpnServiceJava = QAndroidJniObject(toyVpnServiceJava_);
    QString server_host = QAndroidJniObject(server_host_).toString();
    int server_port = server_port_;
    QString secret = QAndroidJniObject(secret_).toString();

    MainThread *service_thread = new MainThread(server_host, server_port, secret);
    service_thread->start();

    __android_log_write(ANDROID_LOG_INFO, "ToyVpnService", "ending of main()");
}

bool protect(int socket) {
    __android_log_write(ANDROID_LOG_INFO, "ToyVpnService", "protecting socket " + QString::number(socket).toUtf8());
    return (bool) toyVpnServiceJava.callMethod<jboolean>("protect", "(I)Z", socket);
}

int configure(int mtu,
              const QString &address,
              int address_mask,
              const QString &route,
              int route_mask,
              const QString &dns_server,
              const QString &search_domain,
              const QHostAddress &server)
{
    return toyVpnServiceJava.callMethod<int>("configure_",
                                             "(ILjava/lang/String;ILjava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)I",
                                             mtu,
                                             QAndroidJniObject::fromString(address).object<jstring>(),
                                             address_mask,
                                             QAndroidJniObject::fromString(route).object<jstring>(),
                                             route_mask,
                                             QAndroidJniObject::fromString(dns_server).object<jstring>(),
                                             QAndroidJniObject::fromString(search_domain).object<jstring>(),
                                             QAndroidJniObject::fromString(server.toString()).object<jstring>());
}


MainThread::MainThread(const QString &server_host,
                           int server_port,
                           const QString &secret)
    : server_host(server_host),
      server_port(server_port),
      secret(secret)
{
}

void MainThread::run() {
    VPNService *service = new VPNService(this->server_host, this->server_port, this->secret);
    service->initialize();
    QThread::run();
}