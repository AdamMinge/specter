#ifndef SPECTER_OBSERVE_TREE_OBSERVER_H
#define SPECTER_OBSERVE_TREE_OBSERVER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
#include <QPointer>
#include <QQueue>
#include <QTimer>
/* ---------------------------------- Standard ------------------------------ */
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
/* ----------------------------------- Local -------------------------------- */
#include "specter/export.h"
#include "specter/observe/tree/action.h"
#include "specter/search/id.h"
#include "specter/search/query.h"
/* -------------------------------------------------------------------------- */

namespace specter {

/* -------------------------------- TreeObserver ---------------------------- */

class LIB_SPECTER_API TreeObserver : public QObject {
  Q_OBJECT

public:
  explicit TreeObserver();
  ~TreeObserver() override;

  void start();
  void stop();

  [[nodiscard]] bool isObserving() const;

Q_SIGNALS:
  void actionReported(const TreeObservedAction &action);

private:
  void startIntervalCheck();
  void stopIntervalCheck();
  void intervalCheck();

  void checkForCreatedObjects();
  void checkForDestroyedObjects();
  void checkForRenamedObjects();
  void checkForReparentedObjects();

  [[nodiscard]] QList<QObject *> getTrackedObjectsInDFSOrder() const;

private:
  struct TrackedObjectCache {
    ObjectId object_id = ObjectId{};
    ObjectQuery object_query = ObjectQuery{};
    QPointer<QObject> object_ptr = nullptr;
    QObject *parent = nullptr;
  };

private:
  bool m_observing;
  QTimer *m_check_timer;
  std::map<QObject *, TrackedObjectCache> m_tracked_objects;
};

/* ----------------------------- TreeObserverQueue ------------------------ */

class LIB_SPECTER_API TreeObserverQueue {
public:
  explicit TreeObserverQueue();
  ~TreeObserverQueue();

  void setObserver(TreeObserver *observer);

  [[nodiscard]] bool isEmpty() const;
  [[nodiscard]] TreeObservedAction popAction();
  [[nodiscard]] TreeObservedAction waitPopAction();

private:
  TreeObserver *m_observer;
  QMetaObject::Connection m_on_action_reported;
  QQueue<TreeObservedAction> m_observed_actions;

  mutable std::mutex m_mutex;
  std::condition_variable m_cv;
};

}// namespace specter

#endif// SPECTER_OBSERVE_TREE_OBSERVER_H